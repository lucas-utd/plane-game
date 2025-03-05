#include "TextureHolder.h"
#include <cassert> // Add this include for assert

void TextureHolder::load(Textures::ID id, const std::string& filename)
{
	std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();
	if (!texture->loadFromFile(filename))
	{
		throw std::runtime_error("TextureHolder::load - Failed to load " + filename);
	}
	auto inserted = textureMap_.insert(std::make_pair(id, std::move(texture)));
	assert(inserted.second && "TextureHolder::load - ID was already loaded");
}

sf::Texture& TextureHolder::get(Textures::ID id)
{
	auto found = textureMap_.find(id);
	assert(found != textureMap_.end() && "TextureHolder::get - ID was not found");
	return *found->second;
}

const sf::Texture& TextureHolder::get(Textures::ID id) const
{
	auto found = textureMap_.find(id);
	assert(found != textureMap_.end() && "TextureHolder::get - ID was not found");
	return *found->second;
}
