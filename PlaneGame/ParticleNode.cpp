#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>

#include "ParticleNode.h"
#include "DataTables.h"
#include "ResourceHolder.h"

namespace 
{
	const std::vector<ParticleData> Table = initializeParticleData();
}

ParticleNode::ParticleNode(Particle::Type type, const TextureHolder& textures)
	: SceneNode()
	, particles_()
	, texture_(textures.get(Textures::Particle))
	, type_(type)
	, vertexArray_(sf::Quads)
	, needsVertexUpdate_(true)
{
}

void ParticleNode::addParticle(sf::Vector2f position)
{
	Particle particle;
	particle.position = position;
	particle.color = Table[type_].color;
	particle.lifetime = Table[type_].lifetime;

	particles_.push_back(particle);
}

Particle::Type ParticleNode::getParticleType() const
{
	return type_;
}

Category ParticleNode::getCategory() const
{
	return Category::ParticleSystem;
}

void ParticleNode::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Remove expired particles at beginning
	while (!particles_.empty() && particles_.front().lifetime <= sf::Time::Zero)
	{
		particles_.pop_front();
	}

	// Decrease lifetime of all particles
	for (Particle& particle : particles_)
	{
		particle.lifetime -= dt;
	}

	needsVertexUpdate_ = true;
}

void ParticleNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (needsVertexUpdate_)
	{
		computeVertices();
		needsVertexUpdate_ = false;
	}

	// Apply particle texture
	states.texture = &texture_;

	// Draw vertices
	target.draw(vertexArray_, states);
}

void ParticleNode::addVertex(float worldX, float worldY, float texCoordX, float texCoordY, sf::Color color) const
{
	sf::Vertex vertex;
	vertex.position = sf::Vector2f(worldX, worldY);
	vertex.color = color;	
	vertex.texCoords = sf::Vector2f(texCoordX, texCoordY);

	vertexArray_.append(vertex);
}

void ParticleNode::computeVertices() const
{
	sf::Vector2f size{ texture_.getSize() };
	sf::Vector2f half = size / 2.f;

	// Refill vertex array
	vertexArray_.clear();
	for (const Particle& particle : particles_)
	{
		sf::Vector2f position = particle.position;
		sf::Color color = particle.color;

		float ratio = particle.lifetime.asSeconds() / Table[type_].lifetime.asSeconds();
		color.a = static_cast<sf::Uint8>(255.f * std::max(ratio, 0.f));

		addVertex(position.x - half.x, position.y - half.y, 0.f, 0.f, color);
		addVertex(position.x + half.x, position.y - half.y, size.x, 0.f, color);
		addVertex(position.x + half.x, position.y + half.y, size.x, size.y, color);
		addVertex(position.x - half.x, position.y + half.y, 0.f, size.y, color);
	}

}
