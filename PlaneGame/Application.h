#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "StateStack.h"
#include "Player.h"

class Application
{
public:
	Application();
	void run();

private:
	void processInput();
	void update(sf::Time dt);
	void render();

	void updateStatistics(sf::Time dt);
	void registerStates();

private:
	static const sf::Time TimePerFrame;
	
	sf::RenderWindow window_;
	TextureHolder textures_;
	FontHolder fonts_;
	Player player_;

	StateStack stateStack_;

	sf::Text statisticsText_;
	sf::Time statisticsUpdateTime_;
	std::size_t statisticsNumFrames_;
};

