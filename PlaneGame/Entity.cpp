#include "Entity.h"

Entity::Entity(int hitpoints)
	: velocity_()
	, hitpoints_(hitpoints)
{
}

void Entity::setVelocity(sf::Vector2f velocity)
{
	velocity_ = velocity;
}

void Entity::setVelocity(float vx, float vy)
{
	velocity_.x = vx;
	velocity_.y = vy;
}

sf::Vector2f Entity::getVelocity() const
{
	return velocity_;
}

void Entity::accelerate(sf::Vector2f velocity)
{
	velocity_ += velocity;
}

void Entity::accelerate(float vx, float vy)
{
	velocity_.x += vx;
	velocity_.y += vy;
}

int Entity::getHitpoints() const
{
	return hitpoints_;
}

void Entity::repair(int points)
{
	assert(points > 0 && "Repairing with negative points");

	hitpoints_ += points;
}

void Entity::damage(int points)
{
	assert(points > 0 && "Damaging with negative points");
	hitpoints_ -= points;
	if (hitpoints_ < 0)
		hitpoints_ = 0;
}

void Entity::destroy()
{
	hitpoints_ = 0;
}

void Entity::remove()
{
	destroy();
}

bool Entity::isDestroyed() const
{
	return hitpoints_ <= 0;
}

void Entity::updateCurrent(sf::Time dt, CommandQueue&)
{
	move(velocity_ * dt.asSeconds());
}
