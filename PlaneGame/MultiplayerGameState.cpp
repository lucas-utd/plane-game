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
	, connected_(false)
	, gameServer_(nullptr)
	, activeState_(true)
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
	playerInvitationText_.setColor(sf::Color::White);
	playerInvitationText_.setString("Press Enter to spawn player 2");
	playerInvitationText_.setPosition(100 - playerInvitationText_.getLocalBounds().width, 760 - playerInvitationText_.getLocalBounds().height);

	// We reuse this text for "Attempt to connect" and "Failed to connect" messages
	failedConnectionText_.setFont(context.fonts->get(Fonts::Main));
	failedConnectionText_.setString("Attempting to connect...");
	failedConnectionText_.setCharacterSize(35);
	failedConnectionText_.setColor(sf::Color::White);
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
		connected_ = true;
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
	if (connected_)
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
	activeState_ = true;
}

void MultiplayerGameState::onDestroy()
{
	if (!isHost_ && connected_)
	{
		// Inform server this client is dying
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::Quit);

		socket_.send(packet);
	}
}

bool MultiplayerGameState::update(sf::Time dt)
{
	// Connected to server: handle all the network logic
	if (connected_)
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
		if (activeState_ && hasFocus_)
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
				connected_ = false;

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
			packet << static_cast<sf::Int32>(Client::GameEvent);
			packet << static_cast<sf::Int32>(gameAction.type);
			packet << gameAction.position.x;
			packet << gameAction.position.y;

			socket_.send(packet);
		}

		// Reguler position update

		if (tickClock_.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			sf::Packet positionUpdatePacket;
			positionUpdatePacket << static_cast<sf::Int32>(Client::PositionUpdate);
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
	activeState_ = false;
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
			packet << static_cast<sf::Int32>(Client::RequestCoopPartner);

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

void MultiplayerGameState::handleEvent(sf::Int32 packetType, sf::Packet& packet)
{
	switch (packetType)
	{
		// Send message to all clients
	case Server::BroadcastMessage:
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

	// Spawn a new player
	default:
		break;
	}
}
