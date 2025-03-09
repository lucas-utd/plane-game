#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "State.h"

class TitleState : public State
{
public:
	TitleState(StateStack& stack, Context context);

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

private:
	sf::Sprite backgroundSprite_;
	sf::Text text_;

	bool isShowText_;
	sf::Time textEffectTime_;
};

