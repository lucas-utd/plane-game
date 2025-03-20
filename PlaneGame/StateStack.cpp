#include <cassert>
#include <format>
#include <sstream>

#include "StateStack.h"

StateStack::StateStack(State::Context context)
	: stack_()
	, pendingList_()
	, context_(context)
	, factories_()
{
}

void StateStack::update(sf::Time dt)
{
	// Iterate from top to bottom, stop as soon as update() returns false
	for (auto itr = stack_.rbegin(); itr != stack_.rend(); ++itr)
	{
		if (!(*itr)->update(dt))
			break;
	}

	applyPendingChanges();
}

void StateStack::draw()
{
	// Draw all active states from bottom to top
	for (const auto& state : stack_)
	{
		state->draw();
	}
}

void StateStack::handleEvent(const sf::Event& event)
{
	// Iterate from top to bottom, stop as soom as handlEvent() returns false
	for (const auto& state : stack_)
	{
		if (!state->handleEvent(event))
			break;
	}

	applyPendingChanges();
}

void StateStack::pushState(States::ID stateID)
{
	pendingList_.push_back(PendingChange(Action::Push, stateID));
}

void StateStack::popState()
{
	pendingList_.push_back(PendingChange(Action::Pop));
}

void StateStack::clearStates()
{
	pendingList_.push_back(PendingChange(Action::Clear));
}

bool StateStack::isEmpty() const
{
	return stack_.empty();
}

State::Ptr StateStack::createState(States::ID stateID)
{
	auto found = factories_.find(stateID);
	std::ostringstream message;
	message << "State not found: " << static_cast<int>(stateID);
	assert(found != factories_.end() && message.str().c_str());
	return found->second();
}

void StateStack::applyPendingChanges()
{
	for (const auto& change : pendingList_)
	{
		switch (change.action)
		{
		case Action::Push:
			stack_.push_back(createState(change.stateID));
			break;

		case Action::Pop:
			stack_.pop_back();
			break;

		case Action::Clear:
			stack_.clear();
			break;
		}
	}

	pendingList_.clear();
}

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
	: action(action)
	, stateID(stateID)
{
}
