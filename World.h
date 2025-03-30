#pragma once

#include <array>
#include <memory>
#include <vector>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>


#include "SceneNode.h"
#include "Aircraft.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "CommandQueue.h"
#include "BloomEffect.h"
#include "SoundPlayer.h"
#include "Pickup.h"
#include "NetworkProtocol.h"
#include "NetworkNode.h"
#include "SpriteNode.h"

// Forward declaration
namespace sf
{
	class RenderTarget;
}

class World : private sf::NonCopyable
{
public:
	World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked = false);
	void update(sf::Time dt);
	void draw();

	sf::FloatRect getViewBounds() const;
	CommandQueue& getCommandQueue();

	Aircraft* addAircraft(int identifier);
	void removeAircraft(int identifier);
	void setCurrentBattleFieldPosition(float lineY);
	void setWorldHeight(float height);

	void addEnemy(Aircraft::Type type, float relX, float relY);
	void sortEnemies();

	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;

	void setWorldScrollCompensation(float compensation);

	Aircraft* getAircraft(int identifier) const;
	sf::FloatRect getBattleFieldBounds() const;

	void createPickup(sf::Vector2f position, Pickup::Type type);
	bool pollGameAction(GameActions::Action& out);

private:
	void loadTextures();
	void adaptPlayerPosition();
	void adaptPlayerVelocity();
	void handleCollisions();
	void updateSounds();
	
	void buildScene();
	void addEnemies();
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();

private:
	enum Layer
	{
		Background,
		LowerAir,
		UpperAir,
		LayerCount
	};

	struct SpawnPoint
	{
		SpawnPoint(Aircraft::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		Aircraft::Type type;
		float x;
		float y;
	};


private:
	sf::RenderTarget& target_;
	sf::RenderTexture sceneTexture_;

	sf::View worldView_;
	TextureHolder textures_;
	FontHolder& fonts_;
	SoundPlayer& sounds_;

	SceneNode sceneGraph_;
	std::array<SceneNode*, LayerCount> sceneLayers_;
	CommandQueue commandQueue_;

	sf::FloatRect worldBounds_;
	sf::Vector2f spawnPosition_;
	float scrollSpeed_;
	float scrollSpeedCompensation_;
	std::vector<Aircraft*> playerAircrafts_;

	std::vector<SpawnPoint> enemySpawnPoints_;
	std::vector<Aircraft*> activeEnemies_;

	BloomEffect bloomEffect_;

	bool networkedWorld;
	NetworkNode* networkNode_;
	SpriteNode* finishSprite_;
};

