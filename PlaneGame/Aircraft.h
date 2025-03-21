#pragma once
#include "Entity.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "CommandQueue.h"
#include "TextNode.h"
#include "Projectile.h"
#include "Animation.h"


class Aircraft :
    public Entity
{
public:
	enum Type
	{
		Eagle,
		Raptor,
		Avenger,
		TypeCount
	};

public:
	explicit Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts);

	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	virtual void remove();
	virtual bool isMarkedForRemoval() const;
	bool isAllied() const;
	float getMaxSpeed() const;

	void increaseFireRate();
	void increaseSpread();
	void collectMissiles(unsigned int count);

	void fire();
	void launchMissile();
	void playLocalSound(CommandQueue& commands, SoundEffect::ID effect);

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateMovementPattern(sf::Time dt);
	void checkPickupDrop(CommandQueue& commands);
	void checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void CreateProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
	void CreatePickup(SceneNode& node, const TextureHolder& textures) const;

	void updateTexts();
	void updateRollAnimation();

private:
	Type type_;
	sf::Sprite sprite_;
	Animation explosion_;
	Command fireCommand_;
	Command missileCommand_;
	sf::Time fireCountdown_;
	bool isFiring_;
	bool isLaunchingMissile_;
	bool isShowExplosion_;
	bool isPlayedExplosionSound_;
	bool isSpawnedPickup_;

	int fireRateLevel_;
	int spreadLevel_;
	int missileAmmo_;

	Command dropPickupCommand_;
	float travelledDistance_;
	std::size_t directionIndex_;
	TextNode* healthDisplay_;
	TextNode* missileDisplay_;
};

