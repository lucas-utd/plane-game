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
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;


// Typedefs for ResourceHolder
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
