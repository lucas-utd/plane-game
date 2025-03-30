#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "StateStack.h"
#include "Player.h"
#include "SoundPlayer.h"
#include "MusicPlayer.h"

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
	MusicPlayer music_;
	SoundPlayer sounds_;

	KeyBinding keyBinding1_;
	KeyBinding keyBinding2_;
	StateStack stateStack_;

	sf::Text statisticsText_;
	sf::Time statisticsUpdateTime_;
	std::size_t statisticsNumFrames_;
};

