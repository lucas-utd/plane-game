#include <SFML/Graphics/RenderWindow.hpp>

#include "SettingsState.h"
#include "Utility.h"
#include "ResourceHolder.h"

SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, guiContainer_()
{
	backgroundSprite_.setTexture(context.textures->get(Textures::TitleScreen));

	// Build key binding buttons and labels
	for (std::size_t x = 0; x != 2; ++x)
	{
		addButtonLabel(PlayerAction::MoveLeft, x, 0, "Move Left", context);
		addButtonLabel(PlayerAction::MoveRight, x, 1, "Move Right", context);
		addButtonLabel(PlayerAction::MoveUp, x, 2, "Move Up", context);
		addButtonLabel(PlayerAction::MoveDown, x, 3, "Move Down", context);
		addButtonLabel(PlayerAction::Fire, x, 4, "Fire", context);
		addButtonLabel(PlayerAction::LaunchMissile, x, 5, "Missile", context);
	}

	updateLabels();

	auto backButton = std::make_shared<GUI::Button>(context);
	backButton->setPosition(80.f, 620.f);
	backButton->setText("Back");
	backButton->setCallback(std::bind(&SettingsState::requestStackPop, this));

	guiContainer_.pack(backButton);
}

void SettingsState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.draw(backgroundSprite_);
	window.draw(guiContainer_);
}

bool SettingsState::update(sf::Time dt)
{
	return true;
}

bool SettingsState::handleEvent(const sf::Event& event)
{
	bool isKeyBinding = false;

	// Iterate through all binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t i = 0; i != 2 * PlayerAction::Count; ++i)
	{
		if (bindingButtons_[i]->isActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				// Player 1
				if (i < PlayerAction::Count)
				{
					getContext().keys1->assignKey(static_cast<PlayerAction>(i), event.key.code);
				}
				// Player 2
				else
				{
					getContext().keys2->assignKey(static_cast<PlayerAction>(i - PlayerAction::Count), event.key.code);
				}

				bindingButtons_[i]->deselect();
			}
			break;
		}
	}

	// If pressed button changed key binding, update labels; otherwise consider other buttons in container
	if (isKeyBinding)
	{
		updateLabels();
	}
	else
	{
		guiContainer_.handleEvent(event);
	}

	return false;
}

void SettingsState::updateLabels()
{
	for (std::size_t i = 0; i != PlayerAction::Count; ++i)
	{
		auto action = static_cast<PlayerAction>(i);

		sf::Keyboard::Key key1 = getContext().keys1->getAssignedKey(action);
		sf::Keyboard::Key key2 = getContext().keys2->getAssignedKey(action);

		bindingLabels_[i]->setText(toString(key1));
		bindingLabels_[i + PlayerAction::Count]->setText(toString(key2));

	}
}

void SettingsState::addButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context)
{
	// For x == 0, start at index 0, otherwise start at half of array
	index += x * PlayerAction::Count;

	bindingButtons_[index] = std::make_shared<GUI::Button>(context);
	bindingButtons_[index]->setPosition(400.f * x + 80.f, 50.f * y + 300.f);
	bindingButtons_[index]->setText(text);
	bindingButtons_[index]->setToggle(true);

	bindingLabels_[index] = std::make_shared<GUI::Label>("", *context.fonts);
	bindingLabels_[index]->setPosition(400.f * x + 300.f, 50.f * y + 315.f);

	guiContainer_.pack(bindingButtons_[index]);
	guiContainer_.pack(bindingLabels_[index]);
}
