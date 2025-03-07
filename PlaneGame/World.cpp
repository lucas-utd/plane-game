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
	playerAircraft_->setVelocity(0.f, 0.f);

	// Forward commands to scene graph
	while (!commandQueue_.isEmpty())
	{
		sceneGraph_.onCommand(commandQueue_.pop(), dt);
	}
	// Adapt player velocity
	adaptPlayerVelocity();

	// Update scene
	sceneGraph_.update(dt);

	// Adapt player position based on velocity
	adaptPlayerPosition();
}

void World::draw()
{
	window_.setView(worldView_);
	window_.draw(sceneGraph_);
}

CommandQueue& World::getCommandQueue()
{
	return commandQueue_;
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
	sceneLayers_[Air]->attachChild(std::move(leader));
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
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
