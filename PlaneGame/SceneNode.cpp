#include <cassert>
#include "SceneNode.h"


SceneNode::SceneNode()
	: parent_(nullptr)
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

void SceneNode::update(sf::Time dt)
{
	updateCurrent(dt);
	updateChildren(dt);
}

void SceneNode::updateCurrent(sf::Time dt)
{
}

void SceneNode::updateChildren(sf::Time dt)
{
	for (const Ptr& child : children_)
	{
		child->update(dt);
	}
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
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
