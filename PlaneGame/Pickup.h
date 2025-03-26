#pragma once

#include "Entity.h"
#include "ResourceIdentifiers.h"

class Aircraft;

class Pickup : public Entity
{
public:
	enum Type
	{
		HealthRefill,
		MissileRefill,
		FireSpread,
		FireRate,
		TypeCount,
	};

public:
	Pickup(Type type, const TextureHolder& textures);

	virtual Category getCategory() const;
	virtual sf::FloatRect getBoundingRect() const ;

	void apply(Aircraft& player) const;

protected:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
	Type type_;
	sf::Sprite sprite_;
};

