#pragma once

#include <queue>

#include "SceneNode.h"
#include "NetworkProtocol.h"

class NetworkNode : public SceneNode
{
public:
	NetworkNode();

	void notifyGameAction(GameActions::Type type, sf::Vector2f position);
	bool pollGameAction(GameActions::Action& out);

	virtual unsigned int getCategory() const override;

private:
	std::queue<GameActions::Action> pendingActions_;
};

