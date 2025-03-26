#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <map>

#include "Command.h"
#include "KeyBinding.h"
#include "NetworkProtocol.h"

class CommandQueue;

class Player : private sf::NonCopyable
{
public:
	typedef PlayerAction Action;

	enum MissionStatus
	{
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};



public:
	Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
	void handleRealtimeNetworkInput(CommandQueue& commands);

	// React to events or realtime state changes received over the network
	void handleNetworkEvent(Action action, CommandQueue& commands);
	void handleNetworkRealtimeChange(Action action, bool actionEnable);

	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;

	void disableAllRealtimeActions();
	bool isLocal() const;

private:
	void initializeActions();

private:
	const KeyBinding* keyBinding_;
	std::map<Action, Command> actionBinding_;
	std::map<Action, bool> actionProxies_;
	MissionStatus currentMissionStatus_;
	int identifier_;
	sf::TcpSocket* socket_;
};

