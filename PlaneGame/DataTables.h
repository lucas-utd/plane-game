#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <functional>

#include "ResourceIdentifiers.h"

class Aircrafe;

struct Direction
{
	Direction(float angle, float distance)
		: angle(angle)
		, distance(distance)
	{
	}

	float angle;
	float distance;
};

struct AircraftData
{
	int hitpoints;
	float speed;
	Textures::ID texture;
	sf::Time fireInterval;
	std::vector<Direction> directions;
};

struct ProjectileData
{
	int damage;
	float speed;
	Textures::ID texture;
};

struct PickupData
{
	std::function<void(Aircrafe&)> action;
	Textures::ID texture;
};

std::vector<AircraftData> initializeAircraftData();
std::vector<ProjectileData> initializeProjectileData();
std::vector<PickupData> initializePickupData();
