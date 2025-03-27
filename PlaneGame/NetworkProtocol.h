#pragma once

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>

const unsigned short ServerPort = 5000;

// Packets originated in the server
enum class ServerPacketType
{
	BroadcastMessage,	// format: [Int32:packetType] [string:message]
	SpawnSelf,			// format: [Int32:packetType]
	InitialState,
	PlayerEvent,
	PlayerRealtimeChange,
	PlayerConnect,
	PlayerDisconnect,
	AcceptCoopPartner,
	SpawnEnemy,
	SpawnPickup,
	UpdateClientState,
	MissionSuccess
};

// Packets originated in the client
enum class ClientPacketType
{
	PlayerEvent,
	PlayerRealtimeChange,
	RequestCoopPartner,
	PositionUpdate,
	GameEvent,
	Quit
};

namespace GameActions
{
	enum Type
	{
		EnemyExplode,
	};

	struct Action
	{
		Action()
		{ // leave uninitialized
		}

		Action(Type type, sf::Vector2f position)
			: type(type)
			, position(position)
		{
		}

		Type			type;
		sf::Vector2f	position;
	};
}
