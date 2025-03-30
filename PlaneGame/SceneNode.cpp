#include <cassert>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "SceneNode.h"
#include "Command.h"


SceneNode::SceneNode(Category category)
	: children_()
	, parent_(nullptr)
	, defaultCategory_(category)
{
}

void SceneNode::attachChild(Ptr child)
{
	child->parent_ = this;
	children_.push_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node)
{
	auto found = std::find_if(children_.begin(), children_.end(),
		[&](Ptr& p) { return p.get() == &node; });

	assert(found != children_.end() && "SceneNode::detachChild - Node not found");

	Ptr result = std::move(*found);
	result->parent_ = nullptr;
	children_.erase(found);
	return result;
}

void SceneNode::update(sf::Time dt, CommandQueue& commands)
{
	updateCurrent(dt, commands);
	updateChildren(dt, commands);
}

void SceneNode::updateCurrent(sf::Time dt, CommandQueue&)
{
}

void SceneNode::updateChildren(sf::Time dt, CommandQueue& commands)
{
	for (const Ptr& child : children_)
	{
		child->update(dt, commands);
	}
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Apply transform of current node
	states.transform *= getTransform();

	// Draw node and children with changed transform
	drawCurrent(target, states);
	drawChildren(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Do nothing by default
}

void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const Ptr& child : children_)
	{
		child->draw(target, states);
	}
}

void SceneNode::drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::FloatRect rect = getBoundingRect();

	sf::RectangleShape shape;
	shape.setPosition(sf::Vector2f(rect.left, rect.top));
	shape.setSize(sf::Vector2f(rect.width, rect.height));
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Green);
	shape.setOutlineThickness(1.f);

	target.draw(shape);
}

sf::Vector2f SceneNode::getWorldPosition() const
{
	return getWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::getWorldTransform() const
{
	sf::Transform transform = sf::Transform::Identity;

	for (const SceneNode* node = this; node != nullptr; node = node->parent_)
	{
		transform = node->getTransform() * transform;
	}

	return transform;
}

void SceneNode::onCommand(const Command& command, sf::Time dt)
{
	// Command current node, if category matches
	if ((command.category & getCategory()) != Category::None)
	{
		command.action(*this, dt);
	}

	// Command children
	for (const Ptr& child : children_)
	{
		child->onCommand(command, dt);
	}
}

Category SceneNode::getCategory() const
{
	return defaultCategory_;
}

void SceneNode::checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs)
{
	checkNodeCollision(sceneGraph, collisionPairs);
	for (const Ptr& child : sceneGraph.children_)
	{
		checkSceneCollision(*child, collisionPairs);
	}
}

void SceneNode::checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs)
{
	if (this != &node && collision(*this, node) && !isDestroyed() && !node.isDestroyed())
	{
		collisionPairs.insert(std::minmax(this, &node));
	}
	for (const Ptr& child : children_)
	{
		child->checkNodeCollision(node, collisionPairs);
	}
}

void SceneNode::removeWrecks()
{
	// Remove all children which request to
	auto wreckFieldBegin = std::remove_if(children_.begin(), children_.end(), std::mem_fn(&SceneNode::isMarkedForRemoval));
	children_.erase(wreckFieldBegin, children_.end());

	// Call function recursively on children
	std::for_each(children_.begin(), children_.end(), std::mem_fn(&SceneNode::removeWrecks));
}

sf::FloatRect SceneNode::getBoundingRect() const
{
	return sf::FloatRect();
}

bool SceneNode::isMarkedForRemoval() const
{
	return isDestroyed();
}

bool SceneNode::isDestroyed() const
{
	return false;
}

bool collision(const SceneNode& lhs, const SceneNode& rhs)
{
	return lhs.getBoundingRect().intersects(rhs.getBoundingRect());
}

float distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return std::hypot(lhs.getWorldPosition().x - rhs.getWorldPosition().x,
		lhs.getWorldPosition().y - rhs.getWorldPosition().y);
}
