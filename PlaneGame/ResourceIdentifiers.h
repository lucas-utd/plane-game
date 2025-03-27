#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
	class Shader;
	class SoundBuffer;
}

enum class Textures
{
	Entities,
	Jungle,
	TitleScreen,
	Buttons,
	Explosion,
	Particle,
	FinishLine,
};

enum class Shaders
{
	BrightnessPass,
	DownSamplePass,
	GaussianBlurPass,
	AddPass,
};

enum class Fonts
{
	Main,
};

enum class SoundEffect
{
	AlliedGunfire,
	EnemyGunfire,
	Explosion1,
	Explosion2,
	LaunchMissile,
	CollectPickup,
	Button,
};

enum Music
{
	MenuTheme,
	MissionTheme,
};


// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;


// Typedefs for ResourceHolder
typedef ResourceHolder<sf::Texture, Textures>			TextureHolder;
typedef ResourceHolder<sf::Font, Fonts>					FontHolder;
typedef ResourceHolder<sf::Shader, Shaders>				ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect>	SoundBufferHolder;
