#pragma once

#include <SFML/Window/Keyboard.hpp>

#include <map>
#include <vector>


enum PlayerAction
{
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	Fire,
	LaunchMissile,
	Count,
};

class KeyBinding
{
public:
	typedef PlayerAction Action;

public:
	explicit KeyBinding(int controlPreconfiguration);

	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;

	bool checkAction(sf::Keyboard::Key key, Action& out) const;
	std::vector<Action> getRealTimeActions() const;

private:
	void initializeActions();

private:
	std::map<sf::Keyboard::Key, Action> keyMap_;
};

bool isRealtimeAction(PlayerAction action);

