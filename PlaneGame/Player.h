#pragma once

#include <map>
#include <SFML/Window/Event.hpp>
#include "Command.h"

class CommandQueue;

class Player
{
public:
	enum Action
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Fire,
		LaunchMissile,
		ActionCount
	};

	enum MissionStatus
	{
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};



public:
	Player();

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);

	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;

	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;

private:
	void initializeActions();
	static bool isRealtimeAction(Action action);

private:
	std::map<sf::Keyboard::Key, Action> keyBinding_;
	std::map<Action, Command> actionBinding_;
	MissionStatus currentMissionStatus_;
};

