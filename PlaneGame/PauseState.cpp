#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "PauseState.h"
#include "ResourceHolder.h"
#include "Utility.h"

PauseState::PauseState(StateStack& stack, Context context)
	: State(stack, context)
	, backgroundSprite_()
	, pausedText_()
	, instructionText_()
{
	sf::Font& font = context.fonts->get(Fonts::ID::Main);
	sf::Vector2f viewSize = context.window->getView().getSize();

	pausedText_.setFont(font);
	pausedText_.setString("Game Paused");
	pausedText_.setCharacterSize(70);
	centerOrigin(pausedText_);
	pausedText_.setPosition(viewSize.x / 2.f, 0.4f * viewSize.y);

	instructionText_.setFont(font);
	instructionText_.setString("Press Escape to return to the game");
	centerOrigin(instructionText_);
	instructionText_.setPosition(viewSize.x / 2.f, 0.6f * viewSize.y);
}

void PauseState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(pausedText_);
	window.draw(instructionText_);
}

bool PauseState::update(sf::Time dt)
{
	return false;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	if (event.type != sf::Event::KeyPressed)
		return false;

	if (event.key.code == sf::Keyboard::Escape)
	{
		// Escape pressed, remove itself to return to the game
		requestStackPop();
	}

	if (event.key.code == sf::Keyboard::BackSpace)
	{
		// Backspace pressed, remove itself to return to the game
		requestStateClear();
		requestStackPush(States::ID::Menu);
	}

	return true;
}
