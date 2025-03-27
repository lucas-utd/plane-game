#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "StateIdentifiers.h"
#include "ResourceIdentifiers.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "KeyBinding.h"


namespace sf
{
	class RenderWindow;
}

class StateStack;
class Player;

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context
	{
		Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts,
			MusicPlayer& music, SoundPlayer& sounds, KeyBinding& keys1, KeyBinding& keys2);

		sf::RenderWindow* window;
		TextureHolder* textures;
		FontHolder* fonts;
		MusicPlayer* music;
		SoundPlayer* sounds;
		KeyBinding* keys1;
		KeyBinding* keys2;
	};

public:
	State(StateStack& stack, Context context);
	virtual ~State();

	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;

	virtual void onActivate();
	virtual void onDestroy();

protected:
	void requestStackPush(States stateID);
	void requestStackPop();
	void requestStateClear();

	Context getContext() const;

private:
	StateStack* stack_;
	Context context_;
};

