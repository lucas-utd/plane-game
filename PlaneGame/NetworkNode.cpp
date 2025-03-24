#include "NetworkNode.h"
#include "NetworkProtocol.h"

NetworkNode::NetworkNode()
	: SceneNode()
	, pendingActions_()
{
}

unsigned int NetworkNode::getCategory() const
{
	return Category::Network;
}

void NetworkNode::notifyGameAction(GameActions::Type type, sf::Vector2f position)
{
	pendingActions_.push(GameActions::Action(type, position));
}

bool NetworkNode::pollGameAction(GameActions::Action& out)
{
	if (pendingActions_.empty())
	{
		return false;
	}
	else
	{
		out = pendingActions_.front();
		pendingActions_.pop();
		return true;
	}
}
