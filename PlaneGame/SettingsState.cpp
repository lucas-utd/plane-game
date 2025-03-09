#include <SFML/Graphics/RenderWindow.hpp>

#include "SettingsState.h"
#include "Utility.h"
#include "ResourceHolder.h"

SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, GUIContainer_()
{
	backgroundSprite_.setTexture(context.textures->get(Textures::ID::TitleScreen));

	// Build key binding buttons and labels
	addButtonLabel(Player::Action::MoveLeft, 150.f, "Move Left", context);
	addButtonLabel(Player::Action::MoveRight, 200.f, "Move Right", context);
	addButtonLabel(Player::Action::MoveUp, 250.f, "Move Up", context);
	addButtonLabel(Player::Action::MoveDown, 300.f, "Move Down", context);

	updateLabels();

	auto backButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	backButton->setPosition(80.f, 375.f);
	backButton->setText("Back");
	backButton->setCallback(std::bind(&SettingsState::requestStackPop, this));

	GUIContainer_.pack(backButton);
}

void SettingsState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.draw(backgroundSprite_);
	window.draw(GUIContainer_);
}

bool SettingsState::update(sf::Time dt)
{
	return true;
}

bool SettingsState::handleEvent(const sf::Event& event)
{
	bool isKeyBinding = false;

	// Iterate through all binding buttons to see if they are being pressed
	for (std::size_t action = 0; action < static_cast<std::size_t>(Player::Action::ActionCount); ++action)
	{
		if (bindingButtons_[action]->isActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				getContext().player->assignKey(static_cast<Player::Action>(action), event.key.code);
				bindingButtons_[action]->deselect();
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
		GUIContainer_.handleEvent(event);
	}

	return false;
}

void SettingsState::updateLabels()
{
	Player& player = *getContext().player;

	for (std::size_t i = 0; i < static_cast<std::size_t>(Player::Action::ActionCount); ++i)
	{
		auto action = static_cast<Player::Action>(i);
		sf::Keyboard::Key key = player.getAssignedKey(action);
		bindingLabels_[i]->setText(toString(key));
	}
}

void SettingsState::addButtonLabel(Player::Action action, float y, const std::string& text, Context context)
{
	size_t index = static_cast<size_t>(action);
	bindingButtons_[index] = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	bindingButtons_[index]->setPosition(80.f, y);
	bindingButtons_[index]->setText(text);
	bindingButtons_[index]->setToggle(true);

	bindingLabels_[index] = std::make_shared<GUI::Label>("", *context.fonts);
	bindingLabels_[index]->setPosition(300.f, y + 15.f);

	GUIContainer_.pack(bindingButtons_[index]);
	GUIContainer_.pack(bindingLabels_[index]);
}
