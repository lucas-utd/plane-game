#pragma once
#include <SFML/System/Time.hpp>
#include <functional>
#include <cassert>

#include "Category.h"

class SceneNode;

struct Command
{
	typedef std::function<void(SceneNode&, sf::Time)> Action;

	Command();

	Action action;
	Category category;
};

template <typename GameObject, typename Function>
Command::Action derivedAction(Function fn)
{
	return [=](SceneNode& node, sf::Time dt)
		{
			// Check if case is safe
			assert(dynamic_cast<GameObject*>(&node) != nullptr);

			// Downcast node to GameObject and invoke action
			fn(static_cast<GameObject&>(node), dt);
		};
}

