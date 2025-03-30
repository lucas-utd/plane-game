#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "State.h"
#include "Container.h"

class PauseState : public State
{
public:
	PauseState(StateStack& stack, Context context, bool letUpdatesThrough = false);
	~PauseState();

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

private:
	sf::Sprite backgroundSprite_;
	sf::Text pausedText_;
	GUI::Container guiContainer_;
	bool isLetUpdatesThrough_;
};

