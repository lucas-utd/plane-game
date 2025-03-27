#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

#include <map>
#include <string>

#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"

class MusicPlayer : private sf::NonCopyable
{
public:
	MusicPlayer();

	void play(Music theme);
	void stop();

	void setPaused(bool paused);
	void setVolume(float volume);

private:
	sf::Music music_;
	std::map<Music, std::string> filenames_;
	float volume_;
};

