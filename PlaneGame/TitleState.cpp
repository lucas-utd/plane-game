#include <SFML/Graphics/RenderWindow.hpp>

#include "TitleState.h"
#include "ResourceHolder.h"
#include "Utility.h"

TitleState::TitleState(StateStack& stack, Context context)
	: State(stack, context)
	, backgroundSprite_()
	, text_()
	, isShowText_(true)
	, textEffectTime_(sf::Time::Zero)
{
	backgroundSprite_.setTexture(context.textures->get(Textures::ID::TitleScreen));

	text_.setFont(context.fonts->get(Fonts::ID::Main));
	text_.setString("Press any key to Start");
	centerOrigin(text_);
	text_.setPosition(context.window->getView().getSize() / 2.f);
}

void TitleState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.draw(backgroundSprite_);

	if (isShowText_)
	{
		window.draw(text_);
	}
}

bool TitleState::update(sf::Time dt)
{
	textEffectTime_ += dt;
	if (textEffectTime_ >= sf::seconds(0.5f))
	{
		isShowText_ = !isShowText_;
		textEffectTime_ = sf::Time::Zero;
	}
	return true;
}

bool TitleState::handleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		requestStackPop();
		requestStackPush(States::ID::Menu);
	}

	return true;
}
