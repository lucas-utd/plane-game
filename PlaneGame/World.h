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


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderWindow& window);
	void update(sf::Time dt);
	void draw();

	CommandQueue& getCommandQueue();


private:
	void loadTextures();
	void buildScene();
	void adaptPlayerPosition();
	void adaptPlayerVelocity();

private:
	enum Layer
	{
		Background,
		Air,
		LayerCount
	};


private:
	sf::RenderWindow& window_;
	sf::View worldView_;
	TextureHolder textures_;

	SceneNode sceneGraph_;
	std::array<SceneNode*, LayerCount> sceneLayers_;
	CommandQueue commandQueue_;

	sf::FloatRect worldBounds_;
	sf::Vector2f spawnPosition_;
	float scrollSpeed_;
	Aircraft* playerAircraft_;
};

