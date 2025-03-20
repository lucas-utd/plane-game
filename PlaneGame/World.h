#pragma once

#include <SFML/Graphics.hpp>
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


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts);
	void update(sf::Time dt);
	void draw();

	CommandQueue& getCommandQueue();

	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;

private:
	void loadTextures();
	void adaptPlayerPosition();
	void adaptPlayerVelocity();
	void handleCollisions();
	
	void buildScene();
	void addEnemies();
	void addEnemy(Aircraft::Type type, float relX, float relY);
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();
	sf::FloatRect getViewBounds() const;
	sf::FloatRect getBattlefieldBounds() const;


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

	SceneNode sceneGraph_;
	std::array<SceneNode*, LayerCount> sceneLayers_;
	CommandQueue commandQueue_;

	sf::FloatRect worldBounds_;
	sf::Vector2f spawnPosition_;
	float scrollSpeed_;
	Aircraft* playerAircraft_;

	std::vector<SpawnPoint> enemySpawnPoints_;
	std::vector<Aircraft*> activeEnemies_;

	BloomEffect bloomEffect_;
};

