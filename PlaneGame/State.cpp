#include "StateStack.h"
#include "State.h"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts,
	MusicPlayer& music, SoundPlayer& sounds, KeyBinding& keys1, KeyBinding& keys2)
	: window(&window)
	, textures(&textures)
	, fonts(&fonts)
	, music(&music)
	, sounds(&sounds)
	, keys1(&keys1)
	, keys2(&keys2)
{
}

State::State(StateStack& stack, Context context)
	: stack_(&stack)
	, context_(context)
{
}

State::~State()
{
}

void State::requestStackPush(States::ID stateID)
{
	stack_->pushState(stateID);
}

void State::requestStackPop()
{
	stack_->popState();
}

void State::requestStateClear()
{
	stack_->clearStates();
}

State::Context State::getContext() const
{
	return context_;
}

void State::onActivate()
{
}

void State::onDestroy()
{
}

