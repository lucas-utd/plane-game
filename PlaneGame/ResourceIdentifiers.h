#pragma once
#include <SFML/Graphics/Font.hpp>

// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
	class Shader;
}

namespace Textures
{
	enum ID
	{
		Entities,
		Jungle,
		TitleScreen,
		Buttons,
		Explosion,
		Particle,
		FinishLine,
	};
}

namespace Shaders
{
	enum ID
	{
		BrightnessPass,
		DownSamplePass,
		GaussianBlurPass,
		AddPass,
	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;


// Typedefs for ResourceHolder
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID> ShaderHolder;
