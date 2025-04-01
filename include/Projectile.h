#pragma once

#include <SFML/Graphics/Sprite.hpp>

#include "Entity.h"
#include "ResourceIdentifiers.h"
#include "CommandQueue.h"

class Projectile
	: public Entity
{
public:
	enum Type
	{
		AlliedBullet,
		EnemyBullet,
		Missile,
		TypeCount
	};
	

public:
	Projectile(Type type, const TextureHolder& textures);

	void guideTowards(sf::Vector2f position);
	bool isGuided() const;

	virtual Category getCategory() const override;
	virtual sf::FloatRect getBoundingRect() const;
	float getMaxSpeed() const;
	int getDamage() const;

private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Type type_;
	sf::Sprite sprite_;
	sf::Vector2f targetDirection_;
};

