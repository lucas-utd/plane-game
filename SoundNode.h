#pragma once

#include "SceneNode.h"
#include "ResourceIdentifiers.h"
#include "SoundPlayer.h"


class SoundNode : public SceneNode
{
public:
	explicit SoundNode(SoundPlayer& player);
	void playSound(SoundEffect sound, sf::Vector2f position);

	virtual Category getCategory() const override;

private:
	SoundPlayer& sounds_;
};

