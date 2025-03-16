#include "Aircraft.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "Utility.h"
#include "DataTables.h"
#include "Pickup.h"

namespace
{
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[type].hitpoints)
	, type_(type)
	, sprite_(textures.get(Table[type].texture))
	, fireCommand_()
	, missileCommand_()
	, fireCountdown_(sf::Time::Zero)
	, isFiring_(false)
	, isLaunchingMissile_(false)
	, isMarkedForRemoval_(false)
	, fireRateLevel_(1)
	, spreadLevel_(1)
	, missileAmmo_(2)
	, dropPickupCommand_()
	, travelledDistance_(0.f)
	, directionIndex_(0)
	, healthDisplay_(nullptr)
	, missileDisplay_(nullptr)
{
	centerOrigin(sprite_);

	fireCommand_.category = Category::SceneAirLayer;
	fireCommand_.action = [this, &textures](SceneNode& node, sf::Time dt) {
		createBullets(node, textures);
		};

	missileCommand_.category = Category::SceneAirLayer;
	missileCommand_.action = [this, &textures](SceneNode& node, sf::Time dt) {
		CreateProjectile(node, Projectile::Type::Missile, 0.f, 0.5f, textures);
		};

	dropPickupCommand_.category = Category::SceneAirLayer;
	dropPickupCommand_.action = [this, &textures](SceneNode& node, sf::Time dt) {
		CreatePickup(node, textures);
		};

	// Set up the health display
	std::unique_ptr<TextNode> healthDisplay{ std::make_unique<TextNode>(fonts, "") };
	healthDisplay_ = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerAircraft)
	{
		std::unique_ptr<TextNode> missileDisplay{ std::make_unique<TextNode>(fonts, "") };
		missileDisplay->setPosition(0.f, 70.f);
		missileDisplay_ = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}


void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);

		isMarkedForRemoval_ = true;
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);

	// Update texts
	updateTexts();
}

unsigned int Aircraft::getCategory() const
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
	return isMarkedForRemoval_;
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

void Aircraft::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[type_].directions;
	if (directions.empty())
	{
		return;
	}

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

	accelerate(vx, vy);

	// Update travelled distance
	travelledDistance_ += getMaxSpeed() * dt.asSeconds();
}

void Aircraft::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0)
	{
		commands.push(dropPickupCommand_);
	}
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
		isLaunchingMissile_ = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

	switch (spreadLevel_)
	{
	case 1:
		CreateProjectile(node, type, 0.f, 0.f, textures);
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
	std::unique_ptr<Projectile> projectile{ std::make_unique<Projectile>(type, textures) };

	sf::Vector2f offset(xOffset * sprite_.getLocalBounds().width, yOffset * sprite_.getLocalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = (isAllied() ? 1.f : -1.f);
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));	

	std::unique_ptr<Pickup> pickup{ std::make_unique<Pickup>(type, textures) };
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts()
{
	healthDisplay_->setString(toString(getHitpoints()) + " HP");
	healthDisplay_->setPosition(0.f, 50.f);
	healthDisplay_->setRotation(-getRotation());

	if (missileDisplay_)
	{
		if (missileAmmo_ == 0)
		{
			missileDisplay_->setString("");
		}
		else
		{
			missileDisplay_->setString("M: " + toString(missileAmmo_));
		}
	}
}
