#include "GameOverState.h"
#include "ResourceHolder.h"
#include "Utility.h"
#include "Player.h"

GameOverState::GameOverState(StateStack& stack, Context context, const std::string& text)
	: State(stack, context)
	, gameOverText_()
	, elapsedTime_(sf::Time::Zero)
{
	sf::Font& font = context.fonts->get(Fonts::Main);
	sf::Vector2f windowSize(context.window->getSize());

	gameOverText_.setFont(font);
	gameOverText_.setString(text);
	gameOverText_.setCharacterSize(70);
	centerOrigin(gameOverText_);
	gameOverText_.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
}

void GameOverState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	// Create dark, semitransparent background
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(gameOverText_);
}

bool GameOverState::update(sf::Time dt)
{
	// Show state for 3 seconds, after returning to menu
	elapsedTime_ += dt;
	if (elapsedTime_ >= sf::seconds(3))
	{
		requestStateClear();
		requestStackPush(States::Menu);
	}
	return false;
}

bool GameOverState::handleEvent(const sf::Event&)
{
	// Ignore all events
	return false;
}
