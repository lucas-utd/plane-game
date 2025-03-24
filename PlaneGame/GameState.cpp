#include "GameState.h"
#include "Player.h"
#include "StateIdentifiers.h"

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, world_(*context.window, *context.fonts, *context.sounds, false)
	, player_(nullptr, 1, context.keys1)
{
	world_.addAircraft(1);
	player_.setMissionStatus(Player::MissionRunning);

	// Play game theme
	context.music->play(Music::MissionTheme);
}

void GameState::draw()
{
	world_.draw();
}

bool GameState::update(sf::Time dt)
{
	world_.update(dt);

	if (!world_.hasAlivePlayer())
	{
		player_.setMissionStatus(Player::MissionFailure);
		requestStackPush(States::GameOver);
	}
	else if (world_.hasPlayerReachedEnd())
	{
		player_.setMissionStatus(Player::MissionSuccess);
		requestStackPush(States::GameOver);
	}

	CommandQueue& commands = world_.getCommandQueue();
	player_.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = world_.getCommandQueue();
	player_.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		requestStackPush(States::Pause);
	}

	return true;
}
