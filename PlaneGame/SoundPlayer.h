#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <list>

#include "ResourceIdentifiers.h"
#include "ResourceHolder.h"


class SoundPlayer : private sf::NonCopyable
{
public:
	SoundPlayer();

	void play(SoundEffect effect);
	void play(SoundEffect effect, sf::Vector2f position);

	void removeStoppedSounds();
	void setListenerPosition(sf::Vector2f position);
	sf::Vector2f getListenerPosition() const;

private:
	SoundBufferHolder soundBuffers_;
	std::list<sf::Sound> sounds_;
};

