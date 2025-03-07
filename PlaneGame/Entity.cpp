#include "Entity.h"

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

void Entity::updateCurrent(sf::Time dt)
{
	move(velocity_ * dt.asSeconds());
}
