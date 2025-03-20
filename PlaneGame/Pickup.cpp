#include <SFML/Graphics/RenderTarget.hpp>

#include "Pickup.h"
#include "DataTables.h"
#include "Aircraft.h"
#include "Utility.h"
#include "CommandQueue.h"


namespace 
{
	const std::vector<PickupData> Table = initializePickupData();
}


Pickup::Pickup(Type type, const TextureHolder& textures)
	: Entity(1)
	, type_(type)
	, sprite_(textures.get(Table[type].texture), Table[type].textureRect)
{
	centerOrigin(sprite_);
}

unsigned int Pickup::getCategory() const
{
	return Category::Pickup;
}

sf::FloatRect Pickup::getBoundingRect() const
{
	return getWorldTransform().transformRect(sprite_.getGlobalBounds());
}

void Pickup::apply(Aircraft& player) const
{
	Table[type_].action(player);
}

void Pickup::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}
