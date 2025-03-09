#pragma once

// Forward declaration of SFML classes
namespace sf
{
	class Texture;
}

namespace Textures
{
	enum class ID
	{
		Eagle,
		Raptor,
		Desert,
		TitleScreen,
		ButtonNormal,
		ButtonSelected,
		ButtonPressed,
	};
}

namespace Fonts
{
	enum class ID
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
