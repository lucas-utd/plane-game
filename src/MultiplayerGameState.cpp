#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <fstream>

#include "MultiplayerGameState.h"
#include "Utility.h"
#include "MusicPlayer.h"


sf::IpAddress getAddressFromFile()
{
	{
		// Try to open existing file (RAII block)
		std::ifstream inputFile("ip.txt");
		std::string ipAddress;
		if (inputFile >> ipAddress)
		{
			return ipAddress;
		}
	}

	// If open/read failed. create new file
	std::ofstream outputFile("ip.txt");
	std::string localAddress = "127.0.0.1";
	outputFile << localAddress;
	return localAddress;
}


MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context context, bool isHost)
	: State(stack, context)
	, world_(*context.window, *context.fonts, *context.sounds, true)
	, window_(*context.window)
	, textureHolder_(*context.textures)
	, isConnected_(false)
	, gameServer_(nullptr)
	, isActiveState_(true)
	, hasFocus_(true)
	, isHost_(isHost)
	, isGameStarted_(false)
	, clientTimeout_(sf::seconds(2.f))
	, timeSinceLastPacket_(sf::Time::Zero)
{
	broadcastText_.setFont(context.fonts->get(Fonts::Main));
	broadcastText_.setPosition(1024.f / 2, 100.f);

	playerInvitationText_.setFont(context.fonts->get(Fonts::Main));
	playerInvitationText_.setCharacterSize(20);
	playerInvitationText_.setFillColor(sf::Color::White);
	playerInvitationText_.setString("Press Enter to spawn player 2");
	playerInvitationText_.setPosition(100 - playerInvitationText_.getLocalBounds().width, 760 - playerInvitationText_.getLocalBounds().height);

	// We reuse this text for "Attempt to connect" and "Failed to connect" messages
	failedConnectionText_.setFont(context.fonts->get(Fonts::Main));
	failedConnectionText_.setString("Attempting to connect...");
	failedConnectionText_.setCharacterSize(35);
	failedConnectionText_.setFillColor(sf::Color::White);
	centerOrigin(failedConnectionText_);
	failedConnectionText_.setPosition(window_.getSize().x / 2.0f, window_.getSize().y / 2.0f);

	// Render a "establishing connection" frame for user feedback
	window_.clear(sf::Color::Black);
	window_.draw(failedConnectionText_);
	window_.display();
	failedConnectionText_.setString("Could not connect to the remote server!");
	centerOrigin(failedConnectionText_);

	sf::IpAddress ip;
	if (isHost)
	{
		gameServer_.reset(new GameServer(sf::Vector2f(window_.getSize())));
		ip = "127.0.0.1";
	}
	else
	{
		ip = getAddressFromFile();
	}

	if (socket_.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done)
	{
		isConnected_ = true;
	}
	else
	{
		failedConnectionClock_.restart();
	}

	socket_.setBlocking(false);

	// Play game theme
	context.music->play(Music::MissionTheme);
}


void MultiplayerGameState::draw()
{
	if (isConnected_)
	{
		world_.draw();

		// Broadcast message in default view
		window_.setView(window_.getDefaultView());

		if (!broadcasts_.empty())
		{
			window_.draw(broadcastText_);
		}

		if (localPlayerIdentifiers_.size() < 2 && playerInvitationTime_ < sf::seconds(0.5f))
		{
			window_.draw(playerInvitationText_);
		}
	}
	else
	{
		window_.draw(failedConnectionText_);
	}
}

void MultiplayerGameState::onActivate()
{
	isActiveState_ = true;
}

void MultiplayerGameState::onDestroy()
{
	if (!isHost_ && isConnected_)
	{
		// Inform server this client is dying
		sf::Packet packet;
		packet << static_cast<sf::Int32>(ClientPacketType::Quit);

		socket_.send(packet);
	}
}

bool MultiplayerGameState::update(sf::Time dt)
{
	// Connected to server: handle all the network logic
	if (isConnected_)
	{
		world_.update(dt);

		// Remove players whose aircrafts were destroyed
		bool foundLocalPlayer = false;
		for (auto itr = players_.begin(); itr != players_.end();)
		{
			if (std::find(localPlayerIdentifiers_.begin(), localPlayerIdentifiers_.end(), itr->first) != localPlayerIdentifiers_.end())
			{
				foundLocalPlayer = true;
			}

			if (!world_.getAircraft(itr->first))
			{
				// Aircraft was destroyed, remove player
				players_.erase(itr++);

				// No more players left:: Mission failed
				if (players_.empty())
				{
					requestStackPush(States::GameOver);
				}
			}
			else
			{
				++itr;
			}
		}

		if (!foundLocalPlayer && isGameStarted_)
		{
			// No local player left, mission failed
			requestStackPush(States::GameOver);
		}

		// Only handle the realtime input if the window has focus and the game is active
		if (isActiveState_ && hasFocus_)
		{
			CommandQueue& commands = world_.getCommandQueue();
			for (auto& player : players_)
			{
				player.second->handleRealtimeNetworkInput(commands);
			}
		}

		// Handle messages from the server that may have arrived
		sf::Packet packet;
		if (socket_.receive(packet) == sf::Socket::Done)
		{
			timeSinceLastPacket_ = sf::Time::Zero;
			sf::Int32 packetType;
			packet >> packetType;
			handlePacket(packetType, packet);
		}
		else
		{
			// Check for timeout with the server
			if (timeSinceLastPacket_ > clientTimeout_)
			{
				isConnected_ = false;

				failedConnectionText_.setString("Lost connection to the server!");
				centerOrigin(failedConnectionText_);

				failedConnectionClock_.restart();
			}
		}

		updateBroadcastMessage(dt);

		// Time counter for blinking 2nd player text
		playerInvitationTime_ += dt;
		if (playerInvitationTime_ > sf::seconds(1.f))
		{
			playerInvitationTime_ = sf::Time::Zero;
		}

		// Events occurring in the game
		GameActions::Action gameAction;
		while (world_.pollGameAction(gameAction))
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ClientPacketType::GameEvent);
			packet << static_cast<sf::Int32>(gameAction.type);
			packet << gameAction.position.x;
			packet << gameAction.position.y;

			socket_.send(packet);
		}

		// Reguler position update

		if (tickClock_.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			sf::Packet positionUpdatePacket;
			positionUpdatePacket << static_cast<sf::Int32>(ClientPacketType::PositionUpdate);
			positionUpdatePacket << static_cast<sf::Int32>(localPlayerIdentifiers_.size());

			for (const auto& identifier : localPlayerIdentifiers_)
			{
				Aircraft* aircraft = world_.getAircraft(identifier);
				if (aircraft)
				{
					positionUpdatePacket << identifier;
					positionUpdatePacket << aircraft->getPosition().x;
					positionUpdatePacket << aircraft->getPosition().y;
					positionUpdatePacket << static_cast<sf::Int32>(aircraft->getHitpoints());
					positionUpdatePacket << static_cast<sf::Int32>(aircraft->getMissileAmmo());
				}
			}

			socket_.send(positionUpdatePacket);
			tickClock_.restart();
		}

		timeSinceLastPacket_ += dt;
	}

	// Failed to connect and waited for more than 5 seconds: back to menu
	else if (failedConnectionClock_.getElapsedTime() > sf::seconds(5.f))
	{
		requestStackPop();
		requestStackPush(States::Menu);
	}

	return true;
}

void MultiplayerGameState::disableAllRealtimeActions()
{
	isActiveState_ = false;
	for (auto& player : players_)
	{
		player.second->disableAllRealtimeActions();
	}
}

bool MultiplayerGameState::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = world_.getCommandQueue();

	// Forward event to all players
	for (auto& player : players_)
	{
		player.second->handleEvent(event, commands);
	}

	if (event.type == sf::Event::KeyPressed)
	{
		// Enter pressed, add second player co-op (only if we are one player)
		if (event.key.code == sf::Keyboard::Return && localPlayerIdentifiers_.size() == 1)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(ClientPacketType::RequestCoopPartner);

			socket_.send(packet);
		}

		// Escape pressed, trigger the pause screen
		else if (event.key.code == sf::Keyboard::Escape)
		{
			disableAllRealtimeActions();
			requestStackPush(States::NetworkPause);
		}
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		hasFocus_ = true;
	}
	else if (event.type == sf::Event::LostFocus)
	{
		hasFocus_ = false;
	}

	return true;
}

void MultiplayerGameState::updateBroadcastMessage(sf::Time elapsedTime)
{
	if (broadcasts_.empty())
	{
		return;
	}

	// Update broadcast timer
	broadcastElaspedTime_ += elapsedTime;
	if (broadcastElaspedTime_ > sf::seconds(2.5f))
	{
		// If message has expired, remove it
		broadcasts_.erase(broadcasts_.begin());

		// Continue to display the next broadcast message
		if (!broadcasts_.empty())
		{
			broadcastText_.setString(broadcasts_.front());
			centerOrigin(broadcastText_);
			broadcastElaspedTime_ = sf::Time::Zero;
		}
	}
}

void MultiplayerGameState::handlePacket(sf::Int32 packetType, sf::Packet& packet)
{
	switch (static_cast<ServerPacketType>(packetType))
	{
		// Send message to all clients
	case ServerPacketType::BroadcastMessage:
	{
		std::string message;
		packet >> message;
		broadcasts_.push_back(message);

		// Just added the first message, display immediately
		if (broadcasts_.size() == 1)
		{
			broadcastText_.setString(broadcasts_.front());
			centerOrigin(broadcastText_);
			broadcastElaspedTime_ = sf::Time::Zero;
		}
	}
	break;

	// Sent by the server to order to spawn player 1 airplane on connect
	case ServerPacketType::SpawnSelf:
	{
		sf::Int32 aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

		Aircraft* aircraft = world_.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);

		players_[aircraftIdentifier].reset(new Player(&socket_, aircraftIdentifier, getContext().keys1));
		localPlayerIdentifiers_.push_back(aircraftIdentifier);

		isGameStarted_ = true;
	}
	break;

	case ServerPacketType::PlayerConnect:
	{
		sf::Int32 aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

		Aircraft* aircraft = world_.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);

		players_[aircraftIdentifier].reset(new Player(&socket_, aircraftIdentifier, nullptr));
	}
	break;

	case ServerPacketType::PlayerDisconnect:
	{
		sf::Int32 aircraftIdentifier;
		packet >> aircraftIdentifier;

		world_.removeAircraft(aircraftIdentifier);
		players_.erase(aircraftIdentifier);
	}
	break;

	case ServerPacketType::InitialState:
	{
		sf::Int32 aircraftCount;
		float worldHeight;
		float currentScroll;
		packet >> worldHeight >> currentScroll;

		world_.setWorldHeight(worldHeight);
		world_.setCurrentBattleFieldPosition(currentScroll);

		packet >> aircraftCount;
		for (sf::Int32 i = 0; i < aircraftCount; ++i)
		{
			sf::Int32 aircraftIdentifier;
			sf::Int32 hitpoints;
			sf::Int32 missileAmmo;
			sf::Vector2f aircraftPosition;
			packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y >> hitpoints >> missileAmmo;

			Aircraft* aircraft = world_.addAircraft(aircraftIdentifier);
			aircraft->setPosition(aircraftPosition);
			aircraft->setHitpoints(hitpoints);
			aircraft->setMissileAmmo(missileAmmo);

			players_[aircraftIdentifier].reset(new Player(&socket_, aircraftIdentifier, nullptr));
		}
	}
	break;

	case ServerPacketType::AcceptCoopPartner:
	{
		sf::Int32 aircraftIdentifier;
		packet >> aircraftIdentifier;

		world_.addAircraft(aircraftIdentifier);
		players_[aircraftIdentifier].reset(new Player(&socket_, aircraftIdentifier, getContext().keys2));
		localPlayerIdentifiers_.push_back(aircraftIdentifier);
	}
	break;

	// Player event (like missile fired) occurs
	case ServerPacketType::PlayerEvent:
	{
		sf::Int32 aircraftIdentifier;
		sf::Int32 action;
		packet >> aircraftIdentifier >> action;

		auto itr = players_.find(aircraftIdentifier);
		if (itr != players_.end())
		{
			itr->second->handleNetworkEvent(static_cast<Player::Action>(action), world_.getCommandQueue());
		}
	}
	break;

	// Player's movement or fire keyboard state changes
	case ServerPacketType::PlayerRealtimeChange:
	{
		sf::Int32 aircraftIdentifier;
		sf::Int32 action;
		bool actionEnabled;
		packet >> aircraftIdentifier >> action >> actionEnabled;
		auto itr = players_.find(aircraftIdentifier);
		if (itr != players_.end())
		{
			itr->second->handleNetworkRealtimeChange(static_cast<Player::Action>(action), actionEnabled);
		}
	}
	break;

	// New enemy to be created
	case ServerPacketType::SpawnEnemy:
	{
		sf::Int32 enemyType;
		float height;
		float relativeX;
		packet >> enemyType >> height >> relativeX;

		world_.addEnemy(static_cast<Aircraft::Type>(enemyType), relativeX, height);
		world_.sortEnemies();
	}
	break;

	// Mission successfully completed
	case ServerPacketType::MissionSuccess:
	{
		requestStackPush(States::MissionSuccess);
	}
	break;

	// Pickup created
	case ServerPacketType::SpawnPickup:
	{
		sf::Int32 type;
		sf::Vector2f position;
		packet >> type >> position.x >> position.y;
		world_.createPickup(position, static_cast<Pickup::Type>(type));
	}
	break;

	//
	case ServerPacketType::UpdateClientState:
	{
		float currentWorldPosition;
		sf::Int32 aircraftCount;
		packet >> currentWorldPosition >> aircraftCount;

		float currentViewPosition = world_.getViewBounds().top + world_.getViewBounds().height;

		// Set the world's scroll compensation according to the client's position
		world_.setWorldScrollCompensation(currentViewPosition / currentWorldPosition);

		for (sf::Int32 i = 0; i < aircraftCount; ++i)
		{
			sf::Int32 aircraftIdentifier;
			sf::Vector2f aircraftPosition;
			packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

			Aircraft* aircraft = world_.getAircraft(aircraftIdentifier);
			bool isLocalPlane = std::find(localPlayerIdentifiers_.begin(), localPlayerIdentifiers_.end(), aircraftIdentifier) != localPlayerIdentifiers_.end();
			if (aircraft && !isLocalPlane)
			{
				sf::Vector2f interpolatedPosition = aircraft->getPosition() + (aircraftPosition - aircraft->getPosition()) * 0.1f;
				aircraft->setPosition(interpolatedPosition);
			}
		}
	}
	break;
	}
}
