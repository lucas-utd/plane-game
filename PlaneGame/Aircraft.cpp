#include "Aircraft.h"

Aircraft::Aircraft(Type type)
	: type_(type)
{
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}
