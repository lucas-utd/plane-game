#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>

#include "State.h"
#include "World.h"
#include "Player.h"
#include "GameServer.h"
#include "NetworkProtocol.h"


class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context, bool isHost);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	virtual void onActivate();
	void onDestroy();

	void disableAllRealtimeActions();

private:
	void updateBroadcastMessage(sf::Time elapsedTime);
	void handlePacket(sf::Int32 packetType, sf::Packet& packet);

private:
	typedef std::unique_ptr<Player> PlayerPtr;

private:
	World world_;
	sf::RenderWindow& window_;
	TextureHolder& textureHolder_;

	std::map<int, PlayerPtr> players_;
	std::vector<sf::Int32> localPlayerIdentifiers_;
	sf::TcpSocket socket_;
	bool isConnected_;
	std::unique_ptr<GameServer> gameServer_;
	sf::Clock tickClock_;

	std::vector<std::string> broadcasts_;
	sf::Text broadcastText_;
	sf::Time broadcastElaspedTime_;

	sf::Text playerInvitationText_;
	sf::Time playerInvitationTime_;

	sf::Text failedConnectionText_;
	sf::Clock failedConnectionClock_;

	bool isActiveState_;
	bool hasFocus_;
	bool isHost_;
	bool isGameStarted_;
	sf::Time clientTimeout_;
	sf::Time timeSinceLastPacket_;
};

