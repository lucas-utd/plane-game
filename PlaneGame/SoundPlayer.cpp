#include "SoundPlayer.h"

#include <SFML/Audio/Listener.hpp>

#include <cmath>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
	: soundBuffers_()
	, sounds_()
{
	soundBuffers_.load(SoundEffect::AlliedGunfire, "Media/Sound/AlliedGunfire.wav");
	soundBuffers_.load(SoundEffect::EnemyGunfire, "Media/Sound/EnemyGunfire.wav");
	soundBuffers_.load(SoundEffect::Explosion1, "Media/Sound/Explosion1.wav");
	soundBuffers_.load(SoundEffect::Explosion2, "Media/Sound/Explosion2.wav");
	soundBuffers_.load(SoundEffect::LaunchMissile, "Media/Sound/LaunchMissile.wav");
	soundBuffers_.load(SoundEffect::CollectPickup, "Media/Sound/CollectPickup.wav");
	soundBuffers_.load(SoundEffect::Button, "Media/Sound/Button.wav");

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::play(SoundEffect effect)
{
	play(effect, getListenerPosition());
}

void SoundPlayer::play(SoundEffect effect, sf::Vector2f position)
{
	sounds_.push_back(sf::Sound());
	sf::Sound& sound = sounds_.back();

	sound.setBuffer(soundBuffers_.get(effect));
	sound.setPosition(position.x, -position.y, 0.f);
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);

	sound.play();
}

void SoundPlayer::removeStoppedSounds()
{
	sounds_.remove_if([](const sf::Sound& sound)
		{
			return sound.getStatus() == sf::Sound::Stopped;
		});
}

void SoundPlayer::setListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::getListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}