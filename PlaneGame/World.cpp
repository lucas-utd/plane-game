#include "SpriteNode.h"
#include "World.h"
#include "Pickup.h"
#include "ParticleNode.h"
#include "SoundNode.h"


World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
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
	, playerAircraft_(nullptr)
	, enemySpawnPoints_()
	, activeEnemies_()
{
	sceneTexture_.create(target_.getSize().x, target_.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	worldView_.setCenter(spawnPosition_);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	worldView_.move(0.f, scrollSpeed_ * dt.asSeconds());
	playerAircraft_->setVelocity(0.f, 0.f);

	// Setup commands to destory entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph
	while (!commandQueue_.isEmpty())
	{
		sceneGraph_.onCommand(commandQueue_.pop(), dt);
	}
	// Adapt player velocity
	adaptPlayerVelocity();

	// Handle collisions
	handleCollisions();

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

bool World::hasAlivePlayer() const
{
	return !playerAircraft_->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
	return !worldBounds_.contains(playerAircraft_->getPosition());
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

	sf::Vector2f position = playerAircraft_->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	playerAircraft_->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = playerAircraft_->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		playerAircraft_->setVelocity(velocity / std::sqrt(2.f));
	}

	// Add scrolling velocity
	playerAircraft_->accelerate(0.f, scrollSpeed_);
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
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
		}
		else if (matchesCategories(pair, Category::EnemyAircraft, Category::AlliedAircraft)
			|| matchesCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft))
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
	// Set listener position to the player's position
	sounds_.setListenerPosition(playerAircraft_->getWorldPosition());

	// Remove unused sounds
	sounds_.removeStoppedSounds();
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i != LayerCount; ++i)
	{
		Category::Type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer = std::make_unique<SceneNode>();
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
	sceneLayers_[Background]->attachChild(std::move(finishSprite));

	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode{ std::make_unique<ParticleNode>(Particle::Smoke, textures_) };
	sceneLayers_[LowerAir]->attachChild(std::move(smokeNode));

	// Add propellant particle node to the scene
	std::unique_ptr<ParticleNode> propellantNode{ std::make_unique<ParticleNode>(Particle::Propellant, textures_) };
	sceneLayers_[LowerAir]->attachChild(std::move(propellantNode));

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode{ std::make_unique<SoundNode>(sounds_) };
	sceneGraph_.attachChild(std::move(soundNode));

	// Add player's aircraft
	std::unique_ptr<Aircraft> player = std::make_unique<Aircraft>(Aircraft::Eagle, textures_, fonts_);
	playerAircraft_ = player.get();
	playerAircraft_->setPosition(spawnPosition_);
	sceneLayers_[UpperAir]->attachChild(std::move(player));

	// Add enemy aircraft
	addEnemies();
}

void World::addEnemies()
{
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
		&& enemySpawnPoints_.back().y > getBattlefieldBounds().top)
	{
		SpawnPoint spawn = enemySpawnPoints_.back();

		std::unique_ptr<Aircraft> enemy{ std::make_unique<Aircraft>(spawn.type, textures_, fonts_) };
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);

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
			if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
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

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some are at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}
