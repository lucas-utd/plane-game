#pragma once
#include <SFML/System/Time.hpp>
#include <functional>
#include <cassert>

#include "Category.h"

class SceneNode;

struct Command
{
	Command();
	std::function<void(SceneNode&, sf::Time)> action;
	unsigned int category;
};

template <typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derviedAction(Function fn)
{
	return [=](SceneNode& node, sf::Time dt)
		{
			// Check if case is safe
			assert(dynamic_cast<GameObject*>(&node) != nullptr);

			// Downcast node to GameObject and invoke action
			fn(static_cast<GameObject&>(node), dt);
		};
}

