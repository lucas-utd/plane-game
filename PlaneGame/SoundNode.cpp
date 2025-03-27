#include "SoundNode.h"
#include "SoundPlayer.h"

SoundNode::SoundNode(SoundPlayer& player)
	: SceneNode()
	, sounds_(player)
{
}

void SoundNode::playSound(SoundEffect effect, sf::Vector2f position)
{
	sounds_.play(effect, position);
}

Category SoundNode::getCategory() const
{
	return Category::SoundEffect;
}
