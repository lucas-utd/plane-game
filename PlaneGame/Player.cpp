#include "Player.h"
#include "CommandQueue.h"
#include "Aircraft.h"


#include <map>
#include <string>
#include <algorithm>

struct AircraftMover
{
	AircraftMover(float vx, float vy)
		: velocity(vx, vy)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		aircraft.accelerate(velocity * aircraft.getMaxSpeed());
	}

	sf::Vector2f velocity;
};

Player::Player()
	: currentMissionStatus_(MissionRunning)
{
	// Set initial key bindings
	keyBinding_[sf::Keyboard::Left] = Action::MoveLeft;
	keyBinding_[sf::Keyboard::Right] = Action::MoveRight;
	keyBinding_[sf::Keyboard::Up] = Action::MoveUp;
	keyBinding_[sf::Keyboard::Down] = Action::MoveDown;
	keyBinding_[sf::Keyboard::Space] = Action::Fire;
	keyBinding_[sf::Keyboard::M] = Action::LaunchMissile;

	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's aircraft
	for (auto& pair : actionBinding_)
	{
		pair.second.category = Category::PlayerAircraft;
	}
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	// Key press event
	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = keyBinding_.find(event.key.code);
		if (found != keyBinding_.end() && !isRealtimeAction(found->second))
		{
			commands.push(actionBinding_[found->second]);
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	for (auto& pair : keyBinding_)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			commands.push(actionBinding_[pair.second]);
		}
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = keyBinding_.begin(); itr != keyBinding_.end();)
	{
		if (itr->second == action)
		{
			keyBinding_.erase(itr++);
		}
		else
		{
			++itr;
		}
	}

	// Insert new binding
	keyBinding_[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	for (const auto& pair : keyBinding_)
	{
		if (pair.second == action)
		{
			return pair.first;
		}
	}

	return sf::Keyboard::Unknown;
}

void Player::setMissionStatus(MissionStatus status)
{
	currentMissionStatus_ = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return currentMissionStatus_;
}

void Player::initializeActions()
{
	actionBinding_[Action::MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-1, 0.f));
	actionBinding_[Action::MoveRight].action = derivedAction<Aircraft>(AircraftMover(+1, 0.f));
	actionBinding_[Action::MoveUp].action = derivedAction<Aircraft>(AircraftMover(0.f, -1));
	actionBinding_[Action::MoveDown].action = derivedAction<Aircraft>(AircraftMover(0.f, +1));
	actionBinding_[Action::Fire].action = derivedAction<Aircraft>([](Aircraft& a, sf::Time)
		{
			a.fire();
		});
	actionBinding_[Action::LaunchMissile].action = derivedAction<Aircraft>([](Aircraft& a, sf::Time)
		{
			a.launchMissile();
		});
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
	case Fire:
		return true;

	default:
		return false;
	}
}
