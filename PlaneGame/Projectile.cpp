#include <SFML/Graphics/RenderTarget.hpp>


#include "Projectile.h"
#include "DataTables.h"
#include "Utility.h"
#include "ResourceHolder.h"
#include "EmitterNode.h"

namespace
{
	const std::vector<ProjectileData> Table = initializeProjectileData();
}

Projectile::Projectile(Type type, const TextureHolder& textures)
	: Entity(1)
	, type_(type)
	, sprite_(textures.get(Table[type].texture), Table[type].textureRect)
	, targetDirection_(0.f, 0.f)
{
	centerOrigin(sprite_);

	// Add particle system for missiles
	if (isGuided())
	{
		std::unique_ptr<EmitterNode> smoke{ std::make_unique<EmitterNode>(Particle::Smoke) };
		smoke->setPosition(0.f, getBoundingRect().height / 2.f);
		attachChild(std::move(smoke));

		std::unique_ptr<EmitterNode> propellant{ std::make_unique<EmitterNode>(Particle::Propellant) };
		propellant->setPosition(0.f, getBoundingRect().height / 2.f);
		attachChild(std::move(propellant));
	}
}

void Projectile::guideTowards(sf::Vector2f position)
{
	assert(isGuided() && "Calling guideTowards on a non-guided projectile");
	targetDirection_ = unitVector(position - getWorldPosition());
}

bool Projectile::isGuided() const
{
	return type_ == Missile;
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (isGuided())
	{
		const float approachRate = 200.f;

		sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * targetDirection_ + getVelocity());
		newVelocity *= getMaxSpeed();
		float angle = std::atan2(newVelocity.y, newVelocity.x);

		setRotation(toDegree(angle) + 90.f);
		setVelocity(newVelocity);
	}

	Entity::updateCurrent(dt, commands);
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}

Category Projectile::getCategory() const
{
	if (type_ == EnemyBullet)
		return Category::EnemyAircraft;
	else
		return Category::AlliedProjectile;
}


sf::FloatRect Projectile::getBoundingRect() const
{
	return getWorldTransform().transformRect(sprite_.getGlobalBounds());
}

float Projectile::getMaxSpeed() const
{
	return Table[type_].speed;
}

int Projectile::getDamage() const
{
	return Table[type_].damage;
}

