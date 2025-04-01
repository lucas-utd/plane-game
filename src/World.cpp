#include <cmath>

#include "SpriteNode.h"
#include "World.h"
#include "Pickup.h"
#include "ParticleNode.h"
#include "SoundNode.h"
#include "Category.h"
#include "ResourceIdentifiers.h"


World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked)
	: target_(outputTarget)
	, sceneTexture_()
	, worldView_(outputTarget.getDefaultView())
	, textures_()
	, fonts_(fonts)
	, sounds_(sounds)
	, sceneGraph_()
	, sceneLayers_()
	, worldBounds_(0.f, 0.f, worldView_.getSize().x, 5000.f)
	, spawnPosition_(worldView_.getSize().x / 2.f, worldBounds_.height - worldView_.getSize().y / 2.f)
	, scrollSpeed_(-50.f)
	, scrollSpeedCompensation_(1.f)
	, playerAircrafts_()
	, enemySpawnPoints_()
	, activeEnemies_()
	, networkedWorld(networked)
	, networkNode_(nullptr)
	, finishSprite_(nullptr)
{
	sceneTexture_.create(target_.getSize().x, target_.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	worldView_.setCenter(spawnPosition_);
}

void World::setWorldScrollCompensation(float compensation)
{
	scrollSpeedCompensation_ = compensation;
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	worldView_.move(0.f, scrollSpeed_ * dt.asSeconds() * scrollSpeedCompensation_);

	for (auto& aircraft : playerAircrafts_)
	{
		aircraft->setVelocity(0.f, 0.f);
	}

	// Setup commands to destory entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt player velocity
	while (!commandQueue_.isEmpty())
	{
		sceneGraph_.onCommand(commandQueue_.pop(), dt);
	}

	// Adapt player velocity
	adaptPlayerVelocity();

	// Handle collisions
	handleCollisions();

	// Remove aircrafts that were destroyed (World::removeWrecks() only destroys the entities, not the pointers in playerAircraft_)
	auto firstToRemove = std::remove_if(playerAircrafts_.begin(), playerAircrafts_.end(), std::mem_fn(&Aircraft::isMarkedForRemoval));
	playerAircrafts_.erase(firstToRemove, playerAircrafts_.end());

	// Remove all destoryed entities, create new ones
	sceneGraph_.removeWrecks();
	spawnEnemies();

	// Update scene
	sceneGraph_.update(dt, commandQueue_);

	// Adapt player position based on velocity
	adaptPlayerPosition();

	updateSounds();
}

void World::draw()
{
	if (PostEffect::isSupported())
	{
		sceneTexture_.clear();
		sceneTexture_.setView(worldView_);
		sceneTexture_.draw(sceneGraph_);
		sceneTexture_.display();
		bloomEffect_.apply(sceneTexture_, target_);
	}
	else
	{
		target_.setView(worldView_);
		target_.draw(sceneGraph_);
	}
}

CommandQueue& World::getCommandQueue()
{
	return commandQueue_;
}

Aircraft* World::getAircraft(int identifier) const
{
	for (const auto& aircraft : playerAircrafts_)
	{
		if (aircraft->getIdentifier() == identifier)
		{
			return aircraft;
		}
	}
	return nullptr;
}

void World::removeAircraft(int identifier)
{
	Aircraft* aircraft = getAircraft(identifier);
	if (aircraft)
	{
		aircraft->destroy();
		playerAircrafts_.erase(std::find(playerAircrafts_.begin(), playerAircrafts_.end(), aircraft));
	}
}

Aircraft* World::addAircraft(int identifier)
{
	std::unique_ptr<Aircraft> player = std::make_unique<Aircraft>(Aircraft::Eagle, textures_, fonts_);
	player->setPosition(worldView_.getCenter());
	player->setIdentifier(identifier);

	playerAircrafts_.push_back(player.get());
	sceneLayers_[UpperAir]->attachChild(std::move(player));
	return playerAircrafts_.back();
}

void World::createPickup(sf::Vector2f position, Pickup::Type type)
{
	std::unique_ptr<Pickup> pickup = std::make_unique<Pickup>(type, textures_);
	pickup->setPosition(position);
	pickup->setVelocity(0.f, 1.f);
	sceneLayers_[UpperAir]->attachChild(std::move(pickup));
}

bool World::pollGameAction(GameActions::Action& out)
{
	return networkNode_->pollGameAction(out);
}

void World::setCurrentBattleFieldPosition(float lineY)
{
	worldView_.setCenter(worldView_.getCenter().x, lineY - worldView_.getSize().y / 2);
	spawnPosition_.y = worldBounds_.height;
}

void World::setWorldHeight(float height)
{
	worldBounds_.height = height;
}

bool World::hasAlivePlayer() const
{
	return playerAircrafts_.size() > 0;
}

bool World::hasPlayerReachedEnd() const
{
	if (Aircraft* aircraft = getAircraft(1))
	{
		return !worldBounds_.contains(aircraft->getPosition());
	}
	else
		return false;
}

void World::loadTextures()
{
	textures_.load(Textures::Entities, "Media/Textures/Entities.png");
	textures_.load(Textures::Jungle, "Media/Textures/Jungle.png");
	textures_.load(Textures::Explosion, "Media/Textures/Explosion.png");
	textures_.load(Textures::Particle, "Media/Textures/Particle.png");
	textures_.load(Textures::FinishLine, "Media/Textures/FinishLine.png");
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds{ getViewBounds() };
	const float borderDistance = 40.f;

	for (auto& aircraft : playerAircrafts_)
	{
		sf::Vector2f position = aircraft->getPosition();
		position.x = std::max(position.x, viewBounds.left + borderDistance);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
		position.y = std::max(position.y, viewBounds.top + borderDistance);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
		aircraft->setPosition(position);
	}
}

void World::adaptPlayerVelocity()
{
	for (auto& aircraft : playerAircrafts_)
	{
		sf::Vector2f velocity = aircraft->getVelocity();

		// If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			aircraft->setVelocity(velocity / std::sqrt(2.f));
		}

		// Add scrolling velocity
		aircraft->accelerate(0.f, scrollSpeed_);
	}
}

bool matchesCategories(SceneNode::Pair& colliders, Category type1, Category type2)
{
	Category category1 = colliders.first->getCategory();
	Category category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if ((type1 & category1) != Category::None && (type2 & category2) != Category::None)
	{
		return true;
	}
	else if ((type1 & category2) != Category::None && (type2 & category1) != Category::None)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	sceneGraph_.checkSceneCollision(sceneGraph_, collisionPairs);

	for (auto pair : collisionPairs)
	{
		if (matchesCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);

			// Collision: Player damage = enemy's remaining hitpoints
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}
		else if (matchesCategories(pair, Category::PlayerAircraft, Category::Pickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Collision: Apply pickup effect to player
			pickup.apply(player);
			pickup.destroy();
			player.playLocalSound(commandQueue_, SoundEffect::CollectPickup);
		}
		else if (matchesCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile)
			|| matchesCategories(pair, Category::PlayerAircraft, Category::EnemyProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			// Apply projectile damage to aircraft
			aircraft.damage(projectile.getDamage());
			projectile.destroy();
		}
	}
}

void World::updateSounds()
{
	sf::Vector2f listenerPosition;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (playerAircrafts_.empty())
	{
		listenerPosition = worldView_.getCenter();
	}
	else
	{
		// 1 or more players -> average position of all aircrafts
		for (const auto& aircraft : playerAircrafts_)
		{
			listenerPosition += aircraft->getWorldPosition();
		}
		listenerPosition /= static_cast<float>(playerAircrafts_.size());
	}

	// Set listener position to the player's position
	sounds_.setListenerPosition(listenerPosition);

	// Remove unused sounds
	sounds_.removeStoppedSounds();
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i != LayerCount; ++i)
	{
		Category category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer = std::make_unique<SceneNode>(category);
		sceneLayers_[i] = layer.get();

		sceneGraph_.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& jungleTexture = textures_.get(Textures::Jungle);
	jungleTexture.setRepeated(true);

	float viewHeight = worldView_.getSize().y;
	sf::IntRect textureRect(worldBounds_);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> jungleSprite = std::make_unique<SpriteNode>(jungleTexture, textureRect);
	jungleSprite->setPosition(worldBounds_.left, worldBounds_.top - viewHeight);
	sceneLayers_[Background]->attachChild(std::move(jungleSprite));

	// Add the finish line to the scene
	sf::Texture& finishTexture = textures_.get(Textures::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite = std::make_unique<SpriteNode>(finishTexture);
	finishSprite->setPosition(0.f, -76.f);
	finishSprite_ = finishSprite.get();
	sceneLayers_[Background]->attachChild(std::move(finishSprite));

	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode = std::make_unique<ParticleNode>(Particle::Smoke, textures_);
	sceneLayers_[LowerAir]->attachChild(std::move(smokeNode));

	// Add propellant particle node to the scene
	std::unique_ptr<ParticleNode> propellantNode = std::make_unique<ParticleNode>(Particle::Propellant, textures_);
	sceneLayers_[LowerAir]->attachChild(std::move(propellantNode));

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode = std::make_unique<SoundNode>(sounds_);
	sceneGraph_.attachChild(std::move(soundNode));

	// Add network node, if necessary
	if (networkNode_)
	{
		std::unique_ptr<NetworkNode> networkNode = std::make_unique<NetworkNode>();
		networkNode_ = networkNode.get();
		sceneGraph_.attachChild(std::move(networkNode));
	}

	// Add enemy aircraft
	addEnemies();
}

void World::addEnemies()
{
	if (networkedWorld)
	{
		return;
	}

	// Add enemies to the spawn point container
	addEnemy(Aircraft::Raptor, 0.f, 500.f);
	addEnemy(Aircraft::Raptor, 0.f, 1000.f);
	addEnemy(Aircraft::Raptor, +100.f, 1150.f);
	addEnemy(Aircraft::Raptor, -100.f, 1150.f);
	addEnemy(Aircraft::Avenger, 70.f, 1500.f);
	addEnemy(Aircraft::Avenger, -70.f, 1500.f);
	addEnemy(Aircraft::Avenger, -70.f, 1710.f);
	addEnemy(Aircraft::Avenger, 70.f, 1700.f);
	addEnemy(Aircraft::Avenger, 30.f, 1850.f);
	addEnemy(Aircraft::Raptor, 300.f, 2200.f);
	addEnemy(Aircraft::Raptor, -300.f, 2200.f);
	addEnemy(Aircraft::Raptor, 0.f, 2200.f);
	addEnemy(Aircraft::Raptor, 0.f, 2500.f);
	addEnemy(Aircraft::Avenger, -300.f, 2700.f);
	addEnemy(Aircraft::Avenger, -300.f, 2700.f);
	addEnemy(Aircraft::Raptor, 0.f, 3000.f);
	addEnemy(Aircraft::Raptor, 250.f, 3250.f);
	addEnemy(Aircraft::Raptor, -250.f, 3250.f);
	addEnemy(Aircraft::Avenger, 0.f, 3500.f);
	addEnemy(Aircraft::Avenger, 0.f, 3700.f);
	addEnemy(Aircraft::Raptor, 0.f, 3800.f);
	addEnemy(Aircraft::Avenger, 0.f, 4000.f);
	addEnemy(Aircraft::Avenger, -200.f, 4200.f);
	addEnemy(Aircraft::Raptor, 200.f, 4200.f);
	addEnemy(Aircraft::Raptor, 0.f, 4400.f);

	sortEnemies();
}

void World::sortEnemies()
{
	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(enemySpawnPoints_.begin(), enemySpawnPoints_.end(), [](SpawnPoint lhs, SpawnPoint rhs)
		{
			return lhs.y < rhs.y;
		});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
	SpawnPoint spawn{ type, spawnPosition_.x + relX, spawnPosition_.y - relY };
	enemySpawnPoints_.push_back(spawn);
}

void World::spawnEnemies()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!enemySpawnPoints_.empty()
		&& enemySpawnPoints_.back().y > getBattleFieldBounds().top)
	{
		SpawnPoint spawn = enemySpawnPoints_.back();

		std::unique_ptr<Aircraft> enemy = std::make_unique<Aircraft>(spawn.type, textures_, fonts_);
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);
		if (networkedWorld)
		{
			enemy->disablePickups();
		}

		sceneLayers_[UpperAir]->attachChild(std::move(enemy));

		// Enemy is spawned, remove from the listt to spawn
		enemySpawnPoints_.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
		{
			if (!getBattleFieldBounds().intersects(e.getBoundingRect()))
			{
				e.remove();
			}
		});

	commandQueue_.push(command);
}

void World::guideMissiles()
{
	// Setup command that stores all enemies in activeEnemies_
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time)
		{
			if (!enemy.isDestroyed())
			{
				activeEnemies_.push_back(&enemy);
			}
		});


	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time)
		{
			// Ignore unguided bullets
			if (!missile.isGuided())
			{
				return;
			}

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			// Find closest enmey
			for (const auto& enemy : activeEnemies_)
			{
				float enemyDistance = distance(missile, *enemy);

				if (enemyDistance < minDistance)
				{
					closestEnemy = enemy;
					minDistance = enemyDistance;
				}
			}

			if (closestEnemy)
			{
				missile.guideTowards(closestEnemy->getWorldPosition());
			}
		});

	// Push commands, reset active enemies
	commandQueue_.push(enemyCollector);
	commandQueue_.push(missileGuider);
	activeEnemies_.clear();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
}

sf::FloatRect World::getBattleFieldBounds() const
{
	// Return view bounds + some are at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}
