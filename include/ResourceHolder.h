#pragma once
#include <map>
#include <string>
#include <memory>
#include <cassert>


// Concept for ensuring the resource has a loadFromFile function
template <typename T>
concept LoadableResource = requires(T t, const std::string & filename)
{
	{ t.loadFromFile(filename) }->std::convertible_to<bool>;
} ||
 requires(T t, const std::string & filename, const std::string& secondArg)
{
	{ t.loadFromFile(filename, secondArg) }->std::convertible_to<bool>;
};

// Concept for ensuring the identifier is usable as a key in a map
template <typename T>
concept MapKey = requires(T t)
{
	{ std::declval<std::map<T, int>>().find(t) };
};

template <LoadableResource Resource, MapKey Identifier>
class ResourceHolder
{
public:
	void load(Identifier id, const std::string& filename);

	void load(Identifier id, const std::string& filename, const std::string& secondParam);

	Resource& get(Identifier id);
	const Resource& get(Identifier id) const;

private:
	void insertResource(Identifier id, std::unique_ptr<Resource> resource);

private:
	std::map<Identifier, std::unique_ptr<Resource>> resourceMap_;
};

#include "ResourceHolder.inl"
