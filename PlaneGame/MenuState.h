#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "State.h"

class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);
	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

	void updateOptionText();

private:
	enum class OptionNames
	{
		Play,
		Exit,
	};

private:
	sf::Sprite backgroundSprite_;
	std::vector<sf::Text> options_;
	std::size_t optionIndex_;
};

