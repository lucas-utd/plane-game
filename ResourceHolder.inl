#pragma once
#include "ResourceHolder.h"
#include <stdexcept>


template <LoadableResource Resource, MapKey Identifier>
void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename)
{
	std::unique_ptr<Resource> resource = std::make_unique<Resource>();
	if (!resource->loadFromFile(filename))
	{
		throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);
	}

	insertResource(id, std::move(resource));
}

template <LoadableResource Resource, MapKey Identifier>
void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename, const std::string& secondParam)
{
	std::unique_ptr<Resource> resource = std::make_unique<Resource>();
	if (!resource->loadFromFile(filename, secondParam))
	{
		throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);
	}

	insertResource(id, std::move(resource));
}

template <LoadableResource Resource, MapKey Identifier>
Resource& ResourceHolder<Resource, Identifier>::get(Identifier id)
{
	auto found = resourceMap_.find(id);
	assert(found != resourceMap_.end() && "ResourceHolder::get - ID was not found");
	return *found->second;
}

template <LoadableResource Resource, MapKey Identifier>
const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
	auto found = resourceMap_.find(id);
	assert(found != resourceMap_.end() && "ResourceHolder::get - ID was not found");
	return *found->second;
}

template <LoadableResource Resource, MapKey Identifier>
void ResourceHolder<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource)
{
	auto inserted = resourceMap_.insert(std::make_pair(id, std::move(resource)));
	assert(inserted.second && "ResourceHolder::insertResource - ID was already loaded");
}

