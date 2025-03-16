#include "Projectile.h"
#include "DataTables.h"
#include "Utility.h"
#include "ResourceHolder.h"

namespace
{
	const std::vector<ProjectileData> Table = initializeProjectileData();
}

Projectile::Projectile(Type type, const TextureHolder& textures)
	: Entity(1)
	, type_(type)
	, sprite_(textures.get(Table[type].texture))
	#
	, targetDirection_(0.f, 0.f)
{
	centerOrigin(sprite_);
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

unsigned int Projectile::getCategory() const
{
	if (isGuided())
		return Category::Projectile;
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

