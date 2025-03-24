#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "State.h"
#include "Container.h"


class GameOverState : public State
{
public:
	GameOverState(StateStack& stack, Context context, const std::string& text);

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

private:
	sf::Text gameOverText_;
	sf::Time elapsedTime_;
};

