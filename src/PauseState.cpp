#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "PauseState.h"
#include "ResourceHolder.h"
#include "Utility.h"
#include "Button.h"

PauseState::PauseState(StateStack& stack, Context context, bool letUpdatesThrough)
	: State(stack, context)
	, backgroundSprite_()
	, pausedText_()
	, guiContainer_()
	, isLetUpdatesThrough_(letUpdatesThrough)
{
	sf::Font& font{ context.fonts->get(Fonts::Main) };
	sf::Vector2f windowSize{ context.window->getSize() };

	pausedText_.setFont(font);
	pausedText_.setString("Game Paused");
	pausedText_.setCharacterSize(70);
	centerOrigin(pausedText_);
	pausedText_.setPosition(windowSize.x / 2.f, 0.4f * windowSize.y);

	auto returnButton = std::make_shared<GUI::Button>(context);
	returnButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 75);
	returnButton->setText("Return");
	returnButton->setCallback([this]()
		{
			requestStackPop();
		});

	auto backToMenuButton = std::make_shared<GUI::Button>(context);
	backToMenuButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 125);
	backToMenuButton->setText("Back to Menu");
	backToMenuButton->setCallback([this]()
		{
			requestStateClear();
			requestStackPush(States::Menu);
		});

	guiContainer_.pack(returnButton);
	guiContainer_.pack(backToMenuButton);

	getContext().music->setPaused(true);
}

PauseState::~PauseState()
{
	getContext().music->setPaused(false);
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
	window.draw(guiContainer_);
}

bool PauseState::update(sf::Time dt)
{
	return isLetUpdatesThrough_;
}

bool PauseState::handleEvent(const sf::Event& event)
{
	guiContainer_.handleEvent(event);

	return false;
}
