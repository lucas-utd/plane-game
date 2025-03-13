#include "DataTables.h"
#include "Aircraft.h"
#include "Projectile.h"
#include "Pickup.h"


using namespace std::placeholders;

std::vector<AircraftData> initializeAircraftData()
{
	std::vector<AircraftData> data(Aircraft::TypeCount);

	data[Aircraft::Eagle].hitpoints = 100;
	data[Aircraft::Eagle].speed = 200.f;
	data[Aircraft::Eagle].fireInterval = sf::seconds(1);
	data[Aircraft::Eagle].texture = Textures::Eagle;

	data[Aircraft::Eagle].directions.push_back(Direction(0, 0));
	data[Aircraft::Eagle].directions.push_back(Direction(-45, 80));
	data[Aircraft::Eagle].directions.push_back(Direction(45, 80));

	data[Aircraft::Raptor].hitpoints = 100;
	data[Aircraft::Raptor].speed = 200.f;
	data[Aircraft::Raptor].texture = Textures::Raptor;
	data[Aircraft::Raptor].directions.push_back(Direction(0, 0));
	data[Aircraft::Raptor].directions.push_back(Direction(-45, 80));
	data[Aircraft::Raptor].directions.push_back(Direction(45, 80));
	data[Aircraft::Raptor].fireInterval = sf::Time::Zero;
	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);
	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 300.f;
	data[Projectile::AlliedBullet].texture = Textures::AlliedBullet;
	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::EnemyBullet;
	data[Projectile::Missile].damage = 20;
	data[Projectile::Missile].speed = 200.f;
	data[Projectile::Missile].texture = Textures::Missile;
	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);
	data[Pickup::HealthRefill].action = std::bind(&Aircraft::repair, _1, 25);
	data[Pickup::HealthRefill].texture = Textures::HealthRefill;
	data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);
	data[Pickup::MissileRefill].texture = Textures::MissileRefill;
	return data;
}
