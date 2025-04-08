#include <cmath>

#include <SFML/Graphics/RenderTarget.hpp>

#include "Aircraft.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "Utility.h"
#include "DataTables.h"
#include "Pickup.h"
#include "SoundNode.h"
#include "NetworkNode.h"


namespace
{
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[type].hitpoints)
	, type_(type)
	, sprite_(textures.get(Table[type].texture), Table[type].textureRect)
	, explosion_(textures.get(Textures::Explosion))
	, fireCommand_()
	, missileCommand_()
	, fireCountdown_(sf::Time::Zero)
	, isFiring_(false)
	, isLaunchingMissile_(false)
	, isShowExplosion_(true)
	, isExplosionBegin_(false)
	, isSpawnedPickup_(false)
	, isPickupsEnabled_(true)
	, fireRateLevel_(1)
	, spreadLevel_(1)
	, missileAmmo_(2)
	, dropPickupCommand_()
	, travelledDistance_(0.f)
	, directionIndex_(0)
	, missileDisplay_(nullptr)
	, identifier_(0)
{
	explosion_.setFrameSize(sf::Vector2i(256, 256));
	explosion_.setNumFrames(16);
	explosion_.setDuration(sf::seconds(1.f));

	centerOrigin(sprite_);
	centerOrigin(explosion_);

	fireCommand_.category = Category::SceneAirLayer;
	fireCommand_.action = [this, &textures](SceneNode& node, sf::Time) {
		createBullets(node, textures);
		};

	missileCommand_.category = Category::SceneAirLayer;
	missileCommand_.action = [this, &textures](SceneNode& node, sf::Time) {
		CreateProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
		};

	dropPickupCommand_.category = Category::SceneAirLayer;
	dropPickupCommand_.action = [this, &textures](SceneNode& node, sf::Time) {
		CreatePickup(node, textures);
		};

	// Set up the health display
	std::unique_ptr<TextNode> healthDisplay = std::make_unique<TextNode>(fonts, "");
	healthDisplay_ = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerAircraft)
	{
		std::unique_ptr<TextNode> missileDisplay = std::make_unique<TextNode>(fonts, "");
		missileDisplay->setPosition(0.f, 70.f);
		missileDisplay_ = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}

int Aircraft::getMissileAmmo() const
{
	return missileAmmo_;
}

void Aircraft::setMissileAmmo(int ammo)
{
	missileAmmo_ = ammo;
}


void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && isShowExplosion_)
	{
		target.draw(explosion_, states);
	}
	else
	{
		target.draw(sprite_, states);
	}
}

void Aircraft::disablePickups()
{
	isPickupsEnabled_ = false;
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();
	updateRollAnimation();

	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		explosion_.update(dt);

		// Play explosion sound only once
		if (!isExplosionBegin_)
		{
			SoundEffect soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playLocalSound(commands, soundEffect);

			// Emit network game action for enemy explosions
			if (!isAllied())
			{
				sf::Vector2f position = getWorldPosition();

				Command command;
				command.category = Category::Network;
				command.action = derivedAction<NetworkNode>([position](NetworkNode& node, sf::Time)
					{
						node.notifyGameAction(GameActions::EnemyExplode, position);
					});

				commands.push(command);
			}

			isExplosionBegin_ = true;
		}
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

Category Aircraft::getCategory() const
{
	if (isAllied())
	{
		return Category::PlayerAircraft;
	}
	else
	{
		return Category::EnemyAircraft;
	}
}

sf::FloatRect Aircraft::getBoundingRect() const
{
	return getWorldTransform().transformRect(sprite_.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const
{
	return isDestroyed() && (explosion_.isFinished() || !isShowExplosion_);
}

void Aircraft::remove()
{
	Entity::remove();
	isShowExplosion_ = false;
}

bool Aircraft::isAllied() const
{
	return type_ == Type::Eagle;
}

float Aircraft::getMaxSpeed() const
{
	return Table[type_].speed;
}

void Aircraft::increaseFireRate()
{
	if (fireRateLevel_ < 10)
	{
		++fireRateLevel_;
	}
}

void Aircraft::increaseSpread()
{
	if (spreadLevel_ < 3)
	{
		++spreadLevel_;
	}
}

void Aircraft::collectMissiles(unsigned int count)
{
	missileAmmo_ += count;
}

void Aircraft::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[type_].fireInterval != sf::Time::Zero)
	{
		isFiring_ = true;
	}
}

void Aircraft::launchMissile()
{
	if (missileAmmo_ > 0)
	{
		isLaunchingMissile_ = true;
		--missileAmmo_;
	}
}

void Aircraft::playLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition](SoundNode& node, sf::Time) {
			node.playSound(effect, worldPosition);
		});

	commands.push(command);
}

int Aircraft::getIdentifier() const
{
	return identifier_;
}

void Aircraft::setIdentifier(int identifier)
{
	identifier_ = identifier;
}

void Aircraft::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[type_].directions;
	if (!directions.empty())
	{
		// Move long enough in the current direction: Change direction
		if (travelledDistance_ > directions[directionIndex_].distance)
		{
			directionIndex_ = (directionIndex_ + 1) % directions.size();
			travelledDistance_ = 0.f;
		}

		// Compute velocity from direction
		float radians = toRadian(directions[directionIndex_].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		// Update travelled distance
		travelledDistance_ += getMaxSpeed() * dt.asSeconds();
	}
}

void Aircraft::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0 && !isSpawnedPickup_ && isPickupsEnabled_)
	{
		commands.push(dropPickupCommand_);
	}

	isSpawnedPickup_ = true;
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
	{
		fire();
	}

	// Check for automatic gunfire, allow only in intervals
	if (isFiring_ && fireCountdown_ <= sf::Time::Zero)
	{
		commands.push(fireCommand_);
		playLocalSound(commands, isAllied() ? SoundEffect::AlliedGunfire : SoundEffect::EnemyGunfire);
		
		fireCountdown_ += Table[type_].fireInterval / (fireRateLevel_ + 1.0f);
		isFiring_ = false;
	}
	else if (fireCountdown_ > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		fireCountdown_ -= dt;
		isFiring_ = false;
	}

	// Check for missile launch
	if (isLaunchingMissile_)
	{
		commands.push(missileCommand_);
		playLocalSound(commands, SoundEffect::LaunchMissile);

		isLaunchingMissile_ = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

	switch (spreadLevel_)
	{
	case 1:
		CreateProjectile(node, type, 0.f, 0.5f, textures);
		break;

	case 2:
		CreateProjectile(node, type, -0.33f, 0.33f, textures);
		CreateProjectile(node, type, 0.33f, 0.33f, textures);
		break;

	case 3:
		CreateProjectile(node, type, -0.5f, 0.33f, textures);
		CreateProjectile(node, type, 0.f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.33f, textures);
		break;

	default:
		break;
	}
}

void Aircraft::CreateProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile = std::make_unique<Projectile>(type, textures);

	sf::Vector2f offset(xOffset * sprite_.getGlobalBounds().width, yOffset * sprite_.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = (isAllied() ? -1.f : 1.f);
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup = std::make_unique<Pickup>(type, textures);
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts()
{
	// Display hitpoints
	if (isDestroyed())
	{
		healthDisplay_->setString("");
	}
	else
	{
		healthDisplay_->setString(toString(getHitpoints()) + " HP");
	}
	healthDisplay_->setPosition(0.f, 50.f);
	healthDisplay_->setRotation(-getRotation());

	if (missileDisplay_)
	{
		if (missileAmmo_ == 0 || isDestroyed())
		{
			missileDisplay_->setString("");
		}
		else
		{
			missileDisplay_->setString("M: " + toString(missileAmmo_));
		}
	}
}

void Aircraft::updateRollAnimation()
{
	if (Table[type_].hasRollAnimation)
	{
		sf::IntRect textureRect = Table[type_].textureRect;

		// Roll left:: Texture rect offset once
		if (getVelocity().x < 0.f)
		{
			textureRect.left += textureRect.width;
		}

		// Roll right: Texture rect offset twice
		if (getVelocity().x > 0.f)
		{
			textureRect.left += textureRect.width * 2;
		}
		sprite_.setTextureRect(textureRect);
	}
}
