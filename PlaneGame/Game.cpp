#include "Game.h"
#include <iostream>

	
const float PlayerSpeed = 100.f;
const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: window_(sf::VideoMode(640, 480), "SFML Application")
	, player_()
{
	if (!texture_.loadFromFile("Media/Textures/Eagle.png"))
	{
		std::cerr << "Error loading texture" << std::endl;
		// handle error
	}
	player_.setTexture(texture_);
	player_.setPosition(100.f, 100.f);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (window_.isOpen())
	{

		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
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
		default:
			break;
		}
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::W)
	{
		isMovingUp_ = isPressed;
	}
	else if (key == sf::Keyboard::S)
	{
		isMovingDown_ = isPressed;
	}
	else if (key == sf::Keyboard::A)
	{
		isMovingLeft_ = isPressed;
	}
	else if (key == sf::Keyboard::D)
	{
		isMovingRigh_ = isPressed;
	}
}

void Game::update(sf::Time deltaTime)
{
	sf::Vector2f movement(0.f, 0.f);
	if (isMovingUp_)
	{
		movement.y -= PlayerSpeed;
	}
	if (isMovingDown_)
	{
		movement.y += PlayerSpeed;
	}
	if (isMovingLeft_)
	{
		movement.x -= PlayerSpeed;
	}
	if (isMovingRigh_)
	{
		movement.x += PlayerSpeed;
	}
	if (movement != sf::Vector2f(0.f, 0.f))
	{
		player_.move(movement * deltaTime.asSeconds());
	}
}

void Game::render()
{
	window_.clear();
	window_.draw(player_);
	window_.display();
}
