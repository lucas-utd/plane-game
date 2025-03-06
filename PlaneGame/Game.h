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

#include "World.h"


class Game : private sf::NonCopyable
{
public:
	Game();
	void run();

private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void render();

	void updateStatistics(sf::Time elapsedTime);
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

private:
	static const sf::Time TimePerFrame;

	sf::RenderWindow window_;
	World world_;

	sf::Font font_;
	sf::Text statisticsText_;
	sf::Time statisticsUpdateTime_;
	std::size_t statisticsNumFrames_;
};

