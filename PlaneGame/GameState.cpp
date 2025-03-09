#include "GameState.h"

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, world_(*context.window)
	, player_(*context.player)
{
}

void GameState::draw()
{
	world_.draw();
}

bool GameState::update(sf::Time dt)
{
	world_.update(dt);

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
		requestStackPush(States::ID::Pause);
	}

	return true;
}
