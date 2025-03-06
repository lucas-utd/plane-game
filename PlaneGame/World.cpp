#include "SpriteNode.h"
#include "World.h"


World::World(sf::RenderWindow& window)
	: window_(window)
	, worldView_(window.getDefaultView())
	, textures_()
	, sceneGraph_()
	, sceneLayers_()
	, worldBounds_(0.f, 0.f, worldView_.getSize().x, 2000.f)
	, spawnPosition_(worldView_.getSize().x / 2.f, worldBounds_.height - worldView_.getSize().y / 2.f)
	, scrollSpeed_(-50.f)
	, playerAircraft_(nullptr)
{
	loadTextures();
	buildScene();

	// Prepare the view
	worldView_.setCenter(spawnPosition_);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	worldView_.move(0.f, scrollSpeed_ * dt.asSeconds());

	// Move the player sidewards (plane)
	sf::Vector2f position = playerAircraft_->getPosition();
	sf::Vector2f velocity = playerAircraft_->getVelocity();

	// If players touches borders, flip its direction
	if (position.x <= worldBounds_.left + 150 ||
		position.x >= worldBounds_.left + worldBounds_.width - 150.f)
	{
		velocity.x = -velocity.x;
		playerAircraft_->setVelocity(velocity);
	}

	// Apply movements
	sceneGraph_.update(dt);
}

void World::draw()
{
	window_.setView(worldView_);
	window_.draw(sceneGraph_);
}

void World::loadTextures()
{
	textures_.load(Textures::ID::Eagle, "Media/Textures/Eagle.png");
	textures_.load(Textures::ID::Raptor, "Media/Textures/Raptor.png");
	textures_.load(Textures::ID::Desert, "Media/Textures/Desert.png");
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		SceneNode::Ptr layer = std::make_unique<SceneNode>();
		sceneLayers_[i] = layer.get();

		sceneGraph_.attachChild(std::move(layer));
	}

	// Prepare the background
	sf::Texture& texture = textures_.get(Textures::ID::Desert);
	sf::IntRect textureRect(worldBounds_);
	texture.setRepeated(true);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite = std::make_unique<SpriteNode>(texture, textureRect);
	backgroundSprite->setPosition(worldBounds_.left, worldBounds_.top);
	sceneLayers_[Background]->attachChild(std::move(backgroundSprite));

	// Add player's aircraft
	std::unique_ptr<Aircraft> leader = std::make_unique<Aircraft>(Aircraft::Type::Eagle, textures_);
	playerAircraft_ = leader.get();
	playerAircraft_->setPosition(spawnPosition_);
	playerAircraft_->setVelocity(40.f, scrollSpeed_);
	sceneLayers_[Air]->attachChild(std::move(leader));

	// Add two escorting aircrafts, placed relatively to the main plane
	std::unique_ptr<Aircraft> leftEscort = std::make_unique<Aircraft>(Aircraft::Type::Raptor, textures_);
	leftEscort->setPosition(-80.f, 50.f);
	playerAircraft_->attachChild(std::move(leftEscort));

	std::unique_ptr<Aircraft> rightEscort = std::make_unique<Aircraft>(Aircraft::Type::Raptor, textures_);
	rightEscort->setPosition(80.f, 50.f);
	playerAircraft_->attachChild(std::move(rightEscort));
}
