#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "State.h"
#include "Container.h"

class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

private:
	sf::Sprite backgroundSprite_;
	GUI::Container GUIContainer_;
};

