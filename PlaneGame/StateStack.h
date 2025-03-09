#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <memory>
#include <vector>
#include <map>

#include "State.h"
#include "StateIdentifiers.h"
#include "ResourceIdentifiers.h"
#include "Player.h"

namespace sf
{
	class Event;
	class RenderWindow;
}

class StateStack : private sf::NonCopyable
{
public:
	enum class Action
	{
		Push,
		Pop,
		Clear,
	};

public:
	explicit StateStack(State::Context context);

	template <typename T>
	void registerState(States::ID stateID);

	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);

	void pushState(States::ID stateID);
	void popState();
	void clearStates();

	bool isEmpty() const;


private:
	State::Ptr createState(States::ID stateID);
	void applyPendingChanges();

private:
	struct PendingChange
	{
		explicit PendingChange(Action action, States::ID stateID = States::ID::None);

		Action action;
		States::ID stateID;
	};

private:
	std::vector<State::Ptr> stack_;
	std::vector<PendingChange> pendingList_;

	State::Context context_;
	std::map<States::ID, std::function<State::Ptr()>> factories_;
};

template <typename T>
void StateStack::registerState(States::ID stateID)
{
	factories_[stateID] = [this]()
		{
			return State::Ptr(std::make_unique<T>(*this, context_));
		};
}

