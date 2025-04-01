#include <SFML/Network/Packet.hpp>

#include <map>
#include <string>
#include <algorithm>

#include "Player.h"
#include "CommandQueue.h"
#include "Aircraft.h"
#include "NetworkProtocol.h"

struct AircraftMover
{
	AircraftMover(float vx, float vy, int identifier)
		: velocity(vx, vy)
		, aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
		{
			aircraft.setVelocity(velocity * aircraft.getMaxSpeed());
		}
	}

	sf::Vector2f velocity;
	int aircraftID;
};

struct AircraftFireTrigger
{
	AircraftFireTrigger(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
		{
			aircraft.fire();
		}
	}

	int aircraftID;
};

struct AircraftMissileTrigger
{
	AircraftMissileTrigger(int identifier)
		: aircraftID(identifier)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		if (aircraft.getIdentifier() == aircraftID)
		{
			aircraft.launchMissile();
		}
	}

	int aircraftID;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding)
	: keyBinding_(binding)
	, currentMissionStatus_(MissionRunning)
	, identifier_(identifier)
	, socket_(socket)
{
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
	// Event
	if (event.type == sf::Event::KeyPressed)
	{
		Action action;
		if (keyBinding_ && keyBinding_->checkAction(event.key.code, action) && !isRealtimeAction(action))
		{
			// Network game: In case of an action is triggered, send it to the server
			if (socket_)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(ClientPacketType::PlayerEvent);
				packet << identifier_;
				packet << static_cast<sf::Int32>(action);
				socket_->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				commands.push(actionBinding_[action]);
			}
		}
	}

	// Realtime change (network game)
	if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && socket_)
	{
		Action action;
		if (keyBinding_ && keyBinding_->checkAction(event.key.code, action) && isRealtimeAction(action))
		{
			// Send realtime change to server
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ClientPacketType::PlayerRealtimeChange);
			packet << identifier_;
			packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::KeyPressed);
			socket_->send(packet);
		}
	}
}

bool Player::isLocal() const
{
	// No key binding means this player is remote
	return keyBinding_ != nullptr;
}

void Player::disableAllRealtimeActions()
{
	for (const auto& action : actionProxies_)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(ClientPacketType::PlayerRealtimeChange);
		packet << identifier_;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;

		socket_->send(packet);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a network game and local player or just a single player game
	if (socket_ && isLocal() || !socket_)
	{
		// Lookup all actions and push corresponding commands to the queue
		std::vector<Action> activeActions = keyBinding_->getRealTimeActions();
		for (const auto& action : activeActions)
		{
			commands.push(actionBinding_[action]);
		}
	}
}

void Player::handleRealtimeNetworkInput(CommandQueue& commands)
{
	// Check if this is a network game and it is not a local player
	if (socket_ && !isLocal())
	{
		// Push commands from the network
		for (const auto& pair : actionProxies_)
		{
			if (pair.second && isRealtimeAction(pair.first))
			{
				commands.push(actionBinding_[pair.first]);
			}
		}
	}
}

void Player::handleNetworkEvent(Action action, CommandQueue& commands)
{
	commands.push(actionBinding_[action]);
}

void Player::handleNetworkRealtimeChange(Action action, bool actionEnable)
{
	actionProxies_[action] = actionEnable;
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
	actionBinding_[Action::MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-1, 0.f, identifier_));
	actionBinding_[Action::MoveRight].action = derivedAction<Aircraft>(AircraftMover(+1, 0.f, identifier_));
	actionBinding_[Action::MoveUp].action = derivedAction<Aircraft>(AircraftMover(0.f, -1, identifier_));
	actionBinding_[Action::MoveDown].action = derivedAction<Aircraft>(AircraftMover(0.f, +1, identifier_));
	actionBinding_[Action::Fire].action = derivedAction<Aircraft>(AircraftFireTrigger(identifier_));
	actionBinding_[Action::LaunchMissile].action = derivedAction<Aircraft>(AircraftMissileTrigger(identifier_));
}
