#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>
#include <map>


class GameServer
{
public:
	explicit GameServer(sf::Vector2f battlefieldSize);
	~GameServer();

	void notifyPlayerSpawn(sf::Int32 aircraftIdentifier);
	void notifyPlayerRealtimeChange(sf::Int32 aircraftIdentifier, sf::Int32 action, bool actionEnabled);
	void notifyPlayerEvent(sf::Int32 aircraftIdentifier, sf::Int32 action);

private:
	// A GameServer RemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();

		sf::TcpSocket socket;
		sf::Time lastPacketTime;
		std::vector<sf::Int32> aircraftIdentifiers;
		bool ready;
		bool timeOut;
	};

	// Structure to store information about current aircraft state
	struct AircraftInfo
	{
		sf::Vector2f position;
		sf::Int32 hitpoints;
		sf::Int32 missileAmmo;
		std::map<sf::Int32, bool> realtimeActions;
	};

	// Unique pointer to the remote peers
	typedef std::unique_ptr<RemotePeer> PeerPtr;


private:
	void setListening(bool enable);
	void executionThread();
	void tick();
	sf::Time now() const;

	void handleIncomingPackets();
	void handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);

	void handleIncomingConnections();
	void handleDisconnections();

	void informWorldState(sf::TcpSocket& socket);
	void broadcastMessage(const std::string& message);
	void sendToAll(sf::Packet& packet);
	void updateClientState();

private:
	sf::Thread thread_;
	sf::Clock clock_;
	sf::TcpListener listenerSocket_;
	bool listeningState_;
	sf::Time clientTimeoutTime_;

	std::size_t maxConnectedPlayers_;
	std::size_t connectedPlayers_;

	float worldHeight_;
	sf::FloatRect battleFieldRect_;
	float battleFieldScrollSpeed_;

	std::size_t aircraftCount_;
	std::map<sf::Int32, AircraftInfo> aircraftInfo_;

	std::vector<PeerPtr> peers_;
	sf::Int32 aircraftIdentifierCounter_;
	bool waitingThreadEnd_;

	sf::Time lastSpawnTime_;
	sf::Time timeForNextSpawn_;
};

