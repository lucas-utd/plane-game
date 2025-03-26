#pragma once

// Entity/scene node category, used to dispatch commands
enum class Category
{
	None = 0,
	SceneAirLayer = 1 << 0,
	PlayerAircraft = 1 << 1,
	AlliedAircraft = 1 << 2,
	EnemyAircraft = 1 << 3,
	Pickup = 1 << 4,
	AlliedProjectile = 1 << 5,
	EnemyProjectile = 1 << 6,
	ParticleSystem = 1 << 7,
	SoundEffect = 1 << 8,
	Network = 1 << 9,

	Aircraft = PlayerAircraft | AlliedAircraft | EnemyAircraft,
	Projectile = AlliedProjectile | EnemyProjectile,
};


// Overload bitwise operator for Category enum
inline Category operator&(Category lhs, Category rhs)
{
	return static_cast<Category>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

inline Category operator|(Category lhs, Category rhs)
{
	return static_cast<Category>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
