#include "MusicPlayer.h"


MusicPlayer::MusicPlayer()
	: music_()
	, filenames_()
	, volume_(100.f)
{
	filenames_[Music::MenuTheme] = "Media/Music/MenuTheme.ogg";
	filenames_[Music::MissionTheme] = "Media/Music/MissionTheme.ogg";
}

void MusicPlayer::play(Music theme)
{
	std::string filename = filenames_[theme];

	if (!music_.openFromFile(filename))
	{
		throw std::runtime_error("Music " + filename + " could not be loaded");
	}

	music_.setVolume(volume_);
	music_.setLoop(true);
	music_.play();
}

void MusicPlayer::stop()
{
	music_.stop();
}

void MusicPlayer::setVolume(float volume)
{
	volume_ = volume;
}

void MusicPlayer::setPaused(bool paused)
{
	if (paused)
	{
		music_.pause();
	}
	else
	{
		music_.play();
	}
}
