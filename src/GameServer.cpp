#include <SFML/Network/Packet.hpp>

#include "GameServer.h"
#include "NetworkProtocol.h"
#include "Utility.h"
#include "Pickup.h"
#include "Aircraft.h"

GameServer::RemotePeer::RemotePeer()
	: ready(false)
	, timeOut(false)
{
	socket.setBlocking(false);
}


GameServer::GameServer(sf::Vector2f battlefieldSize)

	: thread_(&GameServer::executionThread, this)
	, listeningState_(false)
	, clientTimeoutTime_(sf::seconds(3.f))
	, maxConnectedPlayers_(10)
	, connectedPlayers_(0)
	, worldHeight_(5000.f)
	, battleFieldRect_(0.f, worldHeight_ - battlefieldSize.y, battlefieldSize.x, battlefieldSize.y)
	, battleFieldScrollSpeed_(-50.f)
	, aircraftCount_(0)
	, peers_(1)
	, aircraftIdentifierCounter_(1)
	, waitingThreadEnd_(false)
	, lastSpawnTime_(sf::Time::Zero)
	, timeForNextSpawn_(sf::seconds(5.f))
{
	listenerSocket_.setBlocking(false);
	peers_[0].reset(new RemotePeer());
	thread_.launch();
}

GameServer::~GameServer()
{
	waitingThreadEnd_ = true;
	thread_.wait();
}

void GameServer::notifyPlayerRealtimeChange(sf::Int32 aircraftIdentifier, sf::Int32 action, bool actionEnabled)
{
	for (std::size_t i = 0; i != connectedPlayers_; ++i)
	{
		if (peers_[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ServerPacketType::PlayerRealtimeChange) << aircraftIdentifier << action << actionEnabled;

			peers_[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerEvent(sf::Int32 aircraftIdentifier, sf::Int32 action)
{
	for (std::size_t i = 0; i != connectedPlayers_; ++i)
	{
		if (peers_[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ServerPacketType::PlayerEvent) << aircraftIdentifier << action;

			peers_[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerSpawn(sf::Int32 aircraftIdentifier)
{
	for (std::size_t i = 0; i != connectedPlayers_; ++i)
	{
		if (peers_[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ServerPacketType::PlayerConnect) << aircraftIdentifier;
			packet << aircraftInfo_[aircraftIdentifier].position.x << aircraftInfo_[aircraftIdentifier].position.y;

			peers_[i]->socket.send(packet);
		}
	}
}

void GameServer::setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{
		if (!listeningState_)
		{
			listeningState_ = (listenerSocket_.listen(ServerPort) == sf::TcpListener::Done);
		}
	}
	else
	{
		listenerSocket_.close();
		listeningState_ = false;
	}
}

void GameServer::executionThread()
{
	setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!waitingThreadEnd_)
	{
		handleIncomingPackets();
		handleIncomingConnections();

		stepTime += stepClock.getElapsedTime();
		stepClock.restart();

		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Fixed update step
		while (stepTime > stepInterval)
		{
			battleFieldRect_.top += battleFieldScrollSpeed_ * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void GameServer::tick()
{
	updateClientState();

	// Check for mission success = all planes with position.y < offset
	bool allAircraftDone = true;
	for (const auto& pair : aircraftInfo_)
	{
		if (pair.second.position.y > 0.f)
		{
			allAircraftDone = false;
			break;
		}
	}

	if (allAircraftDone)
	{
		sf::Packet missionSuccessPacket;
		missionSuccessPacket << static_cast<sf::Int32>(ServerPacketType::MissionSuccess);
		sendToAll(missionSuccessPacket);
	}

	// Remove IDs of aircraft that have been destroyed (relevant if a client has two, and loses one)
	for (auto it = aircraftInfo_.begin(); it != aircraftInfo_.end();)
	{
		if (it->second.hitpoints <= 0)
		{
			aircraftInfo_.erase(it++);
		}
		else
		{
			++it;
		}
	}

	// Check if its time to attempt to spawn enemies
	if (now() >= timeForNextSpawn_ + lastSpawnTime_)
	{
		// No more enemies are spawned near the end
		if (battleFieldRect_.top > 600.f)
		{
			std::size_t enemyCount = 1u + randomInt(2);
			float spawnCenter = static_cast<float>(randomInt(500) - 250);

			// In case only one enemy is spawned, it appears directly at the spawnCenter
			float planeDistance = 0.f;
			float nextSpawnPosition = spawnCenter;

			// In case there are two enemies being spawned together, each is spawned at each side of the spawnCenter, with a minimum distance
			if (enemyCount == 2)
			{
				planeDistance = static_cast<float>(randomInt(250) + 150);
				nextSpawnPosition = spawnCenter - planeDistance / 2.f;
			}

			// Send the spawn orders to all clients
			for (std::size_t i = 0; i != enemyCount; ++i)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(ServerPacketType::SpawnEnemy);
				packet << static_cast<sf::Int32>(1 + randomInt(Aircraft::TypeCount - 1));
				packet << worldHeight_ - battleFieldRect_.top + 500;
				packet << nextSpawnPosition;

				nextSpawnPosition += planeDistance / 2.f;

				sendToAll(packet);
			}

			lastSpawnTime_ = now();
			timeForNextSpawn_ = sf::milliseconds(2000 + randomInt(6000));
		}
	}
}

sf::Time GameServer::now() const
{
	return clock_.getElapsedTime();
}

void GameServer::handleIncomingPackets()
{
	bool detectedTimeout = false;

	for (auto& peer : peers_)
	{
		sf::Packet packet;
		while (peer->socket.receive(packet) == sf::Socket::Done)
		{
			// Interpret packet and react to it
			handleIncomingPacket(packet, *peer, detectedTimeout);

			// Packet was indeed received, update the ping timer
			peer->lastPacketTime = now();
			packet.clear();
		}

		if (now() >= peer->lastPacketTime + clientTimeoutTime_)
		{
			peer->timeOut = true;
			detectedTimeout = true;
		}
	}

	if (detectedTimeout)
	{
		handleDisconnections();
	}
}

void GameServer::handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout)
{
	sf::Int32 packetType;
	packet >> packetType;

	switch (static_cast<ClientPacketType>(packetType))
	{
	case ClientPacketType::Quit:
	{
		receivingPeer.timeOut = true;
		detectedTimeout = true;
	}
	break;

	case ClientPacketType::PlayerEvent:
	{
		sf::Int32 aircraftIdentifier;
		sf::Int32 action;
		packet >> aircraftIdentifier >> action;

		notifyPlayerEvent(aircraftIdentifier, action);
	}
	break;

	case ClientPacketType::PlayerRealtimeChange:
	{
		sf::Int32 aircraftIdentifier;
		sf::Int32 action;
		bool actionEnabled;
		packet >> aircraftIdentifier >> action >> actionEnabled;
		aircraftInfo_[aircraftIdentifier].realtimeActions[action] = actionEnabled;
		notifyPlayerRealtimeChange(aircraftIdentifier, action, actionEnabled);
	}
	break;

	case ClientPacketType::RequestCoopPartner:
	{
		receivingPeer.aircraftIdentifiers.push_back(aircraftIdentifierCounter_);
		aircraftInfo_[aircraftIdentifierCounter_].position = sf::Vector2f(battleFieldRect_.width / 2, battleFieldRect_.top + battleFieldRect_.height / 2.f);
		aircraftInfo_[aircraftIdentifierCounter_].hitpoints = 100;
		aircraftInfo_[aircraftIdentifierCounter_].missileAmmo = 2;

		sf::Packet requestPacket;
		requestPacket << static_cast<sf::Int32>(ServerPacketType::AcceptCoopPartner);
		requestPacket << aircraftIdentifierCounter_
			<< aircraftInfo_[aircraftIdentifierCounter_].position.x
			<< aircraftInfo_[aircraftIdentifierCounter_].position.y;

		receivingPeer.socket.send(requestPacket);
		++aircraftCount_;

		for (const auto& peer : peers_)
		{
			if (peer.get() != &receivingPeer && peer->ready)
			{
				sf::Packet notifyPacket;
				notifyPacket << static_cast<sf::Int32>(ServerPacketType::PlayerConnect)
					<< aircraftIdentifierCounter_
					<< aircraftInfo_[aircraftIdentifierCounter_].position.x
					<< aircraftInfo_[aircraftIdentifierCounter_].position.y;
				peer->socket.send(notifyPacket);
			}
		}
		aircraftIdentifierCounter_++;
	}
	break;

	case ClientPacketType::PositionUpdate:
	{
		sf::Int32 numAircrafts;
		packet >> numAircrafts;

		for (sf::Int32 i = 0; i < numAircrafts; ++i)
		{
			sf::Int32 aircraftIdentifier;
			sf::Int32 aircraftHitpoints;
			sf::Int32 missileAmmo;
			sf::Vector2f aircraftPosition;
			packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y >> aircraftHitpoints >> missileAmmo;
			aircraftInfo_[aircraftIdentifier].position = aircraftPosition;
			aircraftInfo_[aircraftIdentifier].hitpoints = aircraftHitpoints;
			aircraftInfo_[aircraftIdentifier].missileAmmo = missileAmmo;

		}
	}
	break;

	case ClientPacketType::GameEvent:
	{
		sf::Int32 action;
		float x;
		float y;

		packet >> action >> x >> y;

		// Enemy explodes: With certain probability, drop pickup
		// To avoid multiple messages spawning multiple pickups, only listen to first peer (host)
		if (action == GameActions::EnemyExplode && randomInt(3) == 0 && &receivingPeer == peers_[0].get())
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ServerPacketType::SpawnPickup);
			packet << static_cast<sf::Int32>(Pickup::HealthRefill);
			packet << x << y;

			sendToAll(packet);
		}
	}
	break;
	}
}

void GameServer::updateClientState()
{
	sf::Packet updateClientStatePacket;
	updateClientStatePacket << static_cast<sf::Int32>(ServerPacketType::UpdateClientState);
	updateClientStatePacket << static_cast<float>(battleFieldRect_.top + battleFieldRect_.height);
	updateClientStatePacket << static_cast<sf::Int32>(aircraftInfo_.size());

	for (const auto& pair : aircraftInfo_)
	{
		updateClientStatePacket << pair.first
			<< pair.second.position.x
			<< pair.second.position.y;
	}

	sendToAll(updateClientStatePacket);
}

void GameServer::handleIncomingConnections()
{
	if (!listeningState_)
	{
		return;
	}

	if (listenerSocket_.accept(peers_[connectedPlayers_]->socket) != sf::TcpListener::Done)
	{
		// Order the new client to spawn its own plane ( player 1)
		aircraftInfo_[aircraftIdentifierCounter_].position = sf::Vector2f(battleFieldRect_.width / 2, battleFieldRect_.top + battleFieldRect_.height / 2);
		aircraftInfo_[aircraftIdentifierCounter_].hitpoints = 100;
		aircraftInfo_[aircraftIdentifierCounter_].missileAmmo = 2;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(ServerPacketType::SpawnSelf);
		packet << aircraftIdentifierCounter_;
		packet << aircraftInfo_[aircraftIdentifierCounter_].position.x;
		packet << aircraftInfo_[aircraftIdentifierCounter_].position.y;

		peers_[connectedPlayers_]->aircraftIdentifiers.push_back(aircraftIdentifierCounter_);

		broadcastMessage("New player!");
		informWorldState(peers_[connectedPlayers_]->socket);
		notifyPlayerSpawn(aircraftIdentifierCounter_++);

		peers_[connectedPlayers_]->socket.send(packet);
		peers_[connectedPlayers_]->ready = true;
		peers_[connectedPlayers_]->lastPacketTime = now(); // prevent initial timeouts
		++aircraftCount_;
		++connectedPlayers_;

		if (connectedPlayers_ >= maxConnectedPlayers_)
		{
			setListening(false);
		}
		else
		{
			peers_.push_back(PeerPtr(new RemotePeer()));
		}
	}
}

void GameServer::handleDisconnections()
{
	for (auto itr = peers_.begin(); itr != peers_.end();)
	{
		if ((*itr)->timeOut)
		{
			// Infore everyone of the disconnection, erase
			for (const auto& identifier : (*itr)->aircraftIdentifiers)
			{
				sendToAll(sf::Packet() << static_cast<sf::Int32>(ServerPacketType::PlayerDisconnect) << identifier);
				aircraftInfo_.erase(identifier);
			}

			--connectedPlayers_;
			aircraftCount_ -= (*itr)->aircraftIdentifiers.size();
			
			itr = peers_.erase(itr);

			// Go back to a listening state if needed
			if (connectedPlayers_ < maxConnectedPlayers_)
			{
				peers_.push_back(PeerPtr(new RemotePeer()));
				setListening(true);
			}

			broadcastMessage("An ally has disconnected.");
		}
		else
		{
			++itr;
		}
	}
}

// Tell the newly connected peer about how the world is currently
void GameServer::informWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(ServerPacketType::InitialState);
	packet << worldHeight_ << battleFieldRect_.top + battleFieldRect_.height;
	packet << static_cast<sf::Int32>(aircraftCount_);

	for (std::size_t i = 0; i != connectedPlayers_; ++i)
	{
		if (peers_[i]->ready)
		{
			for (const auto& identifier : peers_[i]->aircraftIdentifiers)
			{
				packet << identifier
					<< aircraftInfo_[identifier].position.x
					<< aircraftInfo_[identifier].position.y
					<< aircraftInfo_[identifier].hitpoints
					<< aircraftInfo_[identifier].missileAmmo;
			}
		}
	}

	socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message)
{
	for (std::size_t i = 0; i != connectedPlayers_; ++i)
	{
		if (peers_[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ServerPacketType::BroadcastMessage)
				<< message;

			peers_[i]->socket.send(packet);
		}
	}
}

void GameServer::sendToAll(sf::Packet& packet)
{
	for (std::size_t i = 0; i != connectedPlayers_; ++i)
	{
		if (peers_[i]->ready)
		{
			peers_[i]->socket.send(packet);
		}
	}
}
