#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "StateStack.h"
#include "World.h"


class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

private:
	World world_;
	Player player_;
};

