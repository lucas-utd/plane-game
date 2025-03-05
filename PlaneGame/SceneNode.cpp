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

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	drawCurrent(target, states);
	for (const Ptr& child : children_)
	{
		child->draw(target, states);
	}
}
