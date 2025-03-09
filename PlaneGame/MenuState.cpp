#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Utility.h"
#include "ResourceHolder.h"
#include "MenuState.h"

MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, backgroundSprite_()
	, options_()
	, optionIndex_(0)
{
	sf::Texture& texture = context.textures->get(Textures::ID::TitleScreen);
	sf::Font& font = context.fonts->get(Fonts::ID::Main);

	backgroundSprite_.setTexture(texture);

	// A simple menu demonstration
	sf::Text playOption;
	playOption.setFont(font);
	playOption.setString("Play");
	centerOrigin(playOption);
	playOption.setPosition(context.window->getView().getSize() / 2.f);
	options_.push_back(playOption);

	sf::Text exitOption;
	exitOption.setFont(font);
	exitOption.setString("Exit");
	centerOrigin(exitOption);
	exitOption.setPosition(playOption.getPosition() + sf::Vector2f(0.f, 30.f));

	updateOptionText();
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());
	window.draw(backgroundSprite_);

	for (const sf::Text& option : options_)
	{
		window.draw(option);
	}
}

bool MenuState::update(sf::Time dt)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	// The demonstration menu logic
	if (event.type != sf::Event::KeyPressed)
		return false;

	if (event.key.code == sf::Keyboard::Return)
	{
		if (optionIndex_ == static_cast<std::size_t>(OptionNames::Play))
		{
			requestStackPop();
			requestStackPush(States::ID::Game);
		}
		else if (optionIndex_ == static_cast<std::size_t>(OptionNames::Exit))
		{
			// The exit option was chosen, by removing the menu state, the application will close
			requestStackPop();
		}
	}
	else if (event.key.code == sf::Keyboard::Up)
	{
		if (optionIndex_ > 0)
		{
			--optionIndex_;
		}
		else
		{
			optionIndex_ = options_.size() - 1;
		}

		updateOptionText();
	}
	else if (event.key.code == sf::Keyboard::Down)
	{
		if (optionIndex_ < options_.size() - 1)
		{
			++optionIndex_;
		}
		else
		{
			optionIndex_ = 0;
		}

		updateOptionText();
	}

	return true;
}

void MenuState::updateOptionText()
{
	if (options_.empty())
		return;

	for (auto& text : options_)
	{
		text.setFillColor(sf::Color::White);
	}

	// Highlight the selected option
	options_[optionIndex_].setFillColor(sf::Color::Red);
}
