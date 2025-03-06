#include "Aircraft.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"

Textures::ID toTextureID(Aircraft::Type type)
{
	switch (type)
	{
	case Aircraft::Type::Eagle:
		return Textures::ID::Eagle;
	case Aircraft::Type::Raptor:
		return Textures::ID::Raptor;
	}
	return Textures::ID::Eagle;
}

Aircraft::Aircraft(Type type, const TextureHolder& textures)
	: type_(type)
	, sprite_(textures.get(toTextureID(type)))
{
	sf::FloatRect bounds = sprite_.getLocalBounds();
	sprite_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}
