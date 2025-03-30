#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <magic_enum/magic_enum.hpp>

#include <array>

#include "State.h"
#include "Container.h"
#include "Button.h"
#include "Label.h"
#include "Player.h"

class SettingsState : public State
{
public:
	SettingsState(StateStack& stack, Context context);

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

private:
	void updateLabels();
	void addButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context);

private:
	sf::Sprite backgroundSprite_;
	GUI::Container guiContainer_;

private:
	std::array<GUI::Button::Ptr, 2 * magic_enum::enum_count<PlayerAction>()> bindingButtons_;
	std::array<GUI::Label::Ptr, 2 * magic_enum::enum_count<PlayerAction>()> bindingLabels_;
};

