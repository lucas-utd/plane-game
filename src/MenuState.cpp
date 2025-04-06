#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Utility.h"
#include "ResourceHolder.h"
#include "MenuState.h"
#include "Button.h"

MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, guiContainer_()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	backgroundSprite_.setTexture(texture);

	auto playButton = std::make_shared<GUI::Button>(context);
	playButton->setPosition(100.f, 300.f);
	playButton->setText("Play");
	playButton->setCallback([this]()
		{
			requestStackPop();
			requestStackPush(States::Game);
		});

	auto hostPlayButton = std::make_shared<GUI::Button>(context);
	hostPlayButton->setPosition(100.f, 350.f);
	hostPlayButton->setText("Host");
	hostPlayButton->setCallback([this]()
		{
			requestStackPop();
			requestStackPush(States::HostGame);
		});

	auto joinPlayButton = std::make_shared<GUI::Button>(context);
	joinPlayButton->setPosition(100.f, 400.f);
	joinPlayButton->setText("Join");
	joinPlayButton->setCallback([this]()
		{
			requestStackPop();
			requestStackPush(States::JoinGame);
		});

	auto settingsButton = std::make_shared<GUI::Button>(context);
	settingsButton->setPosition(100.f, 450.f);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
		{
			requestStackPush(States::Settings);
		});

	auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition(100.f, 500.f);
	exitButton->setText("Exit");
	exitButton->setCallback([this]()
		{
			requestStackPop();
		});

	guiContainer_.pack(playButton);
	guiContainer_.pack(hostPlayButton);
	guiContainer_.pack(joinPlayButton);
	guiContainer_.pack(settingsButton);
	guiContainer_.pack(exitButton);

	// Play menu theme
	context.music->play(Music::MenuTheme);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(backgroundSprite_);
	window.draw(guiContainer_);
}

bool MenuState::update(sf::Time)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	guiContainer_.handleEvent(event);
	return false;
}
