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
	void registerState(States stateID);

	template <typename T, typename Param1>
	void registerState(States stateID, Param1 param1);

	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);

	void pushState(States stateID);
	void popState();
	void clearStates();

	bool isEmpty() const;


private:
	State::Ptr createState(States stateID);
	void applyPendingChanges();

private:
	struct PendingChange
	{
		explicit PendingChange(Action action, States stateID = States::None);

		Action action;
		States stateID;
	};

private:
	std::vector<State::Ptr> stack_;
	std::vector<PendingChange> pendingList_;

	State::Context context_;
	std::map<States, std::function<State::Ptr()>> factories_;
};

template <typename T>
void StateStack::registerState(States stateID)
{
	factories_[stateID] = [this]()
		{
			return State::Ptr(std::make_unique<T>(*this, context_));
		};
}

template <typename T, typename Param1>
void StateStack::registerState(States stateID, Param1 arg1)
{
	factories_[stateID] = [this, arg1]()
		{
			return State::Ptr(std::make_unique<T>(*this, context_, arg1));
		};
}

