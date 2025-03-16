#include "GameOverState.h"
#include "ResourceHolder.h"
#include "Utility.h"
#include "Player.h"

GameOverState::GameOverState(StateStack& stack, Context context)
	: State(stack, context)
	, gameOverText_()
	, elapsedTime_()
{
	sf::Font& font = context.fonts->get(Fonts::Main);
	sf::Vector2f windowSize(context.window->getSize());

	gameOverText_.setFont(font);
	if (context.player->getMissionStatus() == Player::MissionFailure)
	{
		gameOverText_.setString("Mission Failed");
	}
	else
	{
		gameOverText_.setString("Mission Complete");
	}

	gameOverText_.setCharacterSize(70);
	centerOrigin(gameOverText_);
	gameOverText_.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
}

void GameOverState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	// Create dark, semitransparent background
	sf::RectangleShape backgroudnShape;
	backgroudnShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroudnShape.setSize(window.getView().getSize());

	window.draw(backgroudnShape);
	window.draw(gameOverText_);
}

bool GameOverState::update(sf::Time dt)
{
	// Show state for 3 seconds, after returning to menu
	elapsedTime_ += dt;
	if (elapsedTime_ >= sf::seconds(3))
	{
		requestStackPop();
		requestStackPush(States::Menu);
	}
	return false;
}

bool GameOverState::handleEvent(const sf::Event& event)
{
	// Ignore all events
	return false;
}
