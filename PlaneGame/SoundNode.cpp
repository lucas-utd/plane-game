#include "SoundNode.h"
#include "SoundPlayer.h"

SoundNode::SoundNode(SoundPlayer& player)
	: SceneNode()
	, sounds_(player)
{
}

void SoundNode::playSound(SoundEffect::ID effect, sf::Vector2f position)
{
	sounds_.play(effect, position);
}

unsigned int SoundNode::getCategory() const
{
	return Category::SoundEffect;
}
