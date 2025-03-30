#include "KeyBinding.h"

#include <string>
#include <algorithm>

KeyBinding::KeyBinding(int controlPreconfiguration)
	: keyMap_()
{
	// Set initial key bindings for player 1
	if (controlPreconfiguration == 1)
	{
		keyMap_[sf::Keyboard::Left] = PlayerAction::MoveLeft;
		keyMap_[sf::Keyboard::Right] = PlayerAction::MoveRight;
		keyMap_[sf::Keyboard::Up] = PlayerAction::MoveUp;
		keyMap_[sf::Keyboard::Down] = PlayerAction::MoveDown;
		keyMap_[sf::Keyboard::Space] = PlayerAction::Fire;
		keyMap_[sf::Keyboard::M] = PlayerAction::LaunchMissile;
	}
	else if (controlPreconfiguration == 2)
	{
		// Player 2
		keyMap_[sf::Keyboard::A] = PlayerAction::MoveLeft;
		keyMap_[sf::Keyboard::D] = PlayerAction::MoveRight;
		keyMap_[sf::Keyboard::W] = PlayerAction::MoveUp;
		keyMap_[sf::Keyboard::S] = PlayerAction::MoveDown;
		keyMap_[sf::Keyboard::F] = PlayerAction::Fire;
		keyMap_[sf::Keyboard::R] = PlayerAction::LaunchMissile;
	}
}

void KeyBinding::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = keyMap_.begin(); itr != keyMap_.end();)
	{
		if (itr->second == action)
		{
			keyMap_.erase(itr++);
		}
		else
		{
			++itr;
		}
	}

	// Insert new binding
	keyMap_[key] = action;
}


sf::Keyboard::Key KeyBinding::getAssignedKey(Action action) const
{
	auto found = std::find_if(keyMap_.rbegin(), keyMap_.rend(), [&](auto& pair) {
		return pair.second == action;
		});

	if (found != keyMap_.rend())
	{
		return found->first;
	}
	
	return sf::Keyboard::Unknown;
}

bool KeyBinding::checkAction(sf::Keyboard::Key key, Action& out) const
{
	auto found = keyMap_.find(key);
	if (found == keyMap_.end())
	{
		return false;
	}
	else
	{
		out = found->second;
		return true;
	}
}


std::vector<KeyBinding::Action> KeyBinding::getRealTimeActions() const
{
	// Return all realtime actions that are currently active.
	std::vector<Action> actions;

	for (const auto& pair : keyMap_)
	{
		// If key is pressed and an action is a realtime action, store it
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			actions.push_back(pair.second);
		}
	}
	return actions;
}


bool isRealtimeAction(PlayerAction action)
{
	switch (action)
	{
	case PlayerAction::MoveLeft:
	case PlayerAction::MoveRight:
	case PlayerAction::MoveUp:
	case PlayerAction::MoveDown:
	case PlayerAction::Fire:
		return true;
	default:
		return false;
	}
}
