#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Utility.h"
#include "ResourceHolder.h"
#include "MenuState.h"
#include "Button.h"

MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, GUIContainer_()
{
	sf::Texture& texture = context.textures->get(Textures::ID::TitleScreen);
	backgroundSprite_.setTexture(texture);

	auto playButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	playButton->setPosition(100.f, 300.f);
	playButton->setText("Play");
	playButton->setCallback([this]()
		{
			requestStackPop();
			requestStackPush(States::ID::Game);
		});

	auto settingsButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	settingsButton->setPosition(100.f, 350.f);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
		{
			requestStackPush(States::ID::Settings);
		});

	auto exitButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	exitButton->setPosition(100.f, 400.f);
	exitButton->setText("Exit");
	exitButton->setCallback([this]()
		{
			requestStackPop();
		});

	GUIContainer_.pack(playButton);
	GUIContainer_.pack(settingsButton);
	GUIContainer_.pack(exitButton);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(backgroundSprite_);
	window.draw(GUIContainer_);
}

bool MenuState::update(sf::Time dt)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	GUIContainer_.handleEvent(event);
	return false;
}
