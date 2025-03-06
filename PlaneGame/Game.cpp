#include "Game.h"
#include <iostream>

	
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: window_(sf::VideoMode(640, 480), "World", sf::Style::Close)
	, world_(window_)
	, font_()
	, statisticsText_()
	, statisticsUpdateTime_()
	, statisticsNumFrames_(0)
{
	font_.loadFromFile("Media/Sansation.ttf");
	statisticsText_.setFont(font_);
	statisticsText_.setPosition(5.f, 5.f);
	statisticsText_.setCharacterSize(10);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (window_.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (window_.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			window_.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime)
{
	world_.update(elapsedTime);
}

void Game::render()
{
	window_.clear();
	world_.draw();

	window_.setView(window_.getDefaultView());
	window_.draw(statisticsText_);
	window_.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	statisticsUpdateTime_ += elapsedTime;
	statisticsNumFrames_ += 1;
	if (statisticsUpdateTime_ >= sf::seconds(1.0f))
	{
		statisticsText_.setString(
			"Frames / Second = " + std::to_string(statisticsNumFrames_) + "\n" +
			"Time / Update = " + std::to_string(statisticsUpdateTime_.asMicroseconds() / statisticsNumFrames_) + "us");
		statisticsUpdateTime_ -= sf::seconds(1.0f);
		statisticsNumFrames_ = 0;
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
}
