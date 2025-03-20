#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include <deque>

#include "SceneNode.h"
#include "Particle.h"
#include "ResourceIdentifiers.h"


class ParticleNode : public SceneNode
{
public:
	ParticleNode(Particle::Type type, const TextureHolder& textures);
	
	void addParticle(sf::Vector2f position);
	Particle::Type getParticleType() const;
	virtual unsigned int getCategory() const override;

private:
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands) override;
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

	void addVertex(float worldX, float worldY, float texCoordX, float texCoordY, sf::Color color) const;
	void computeVertices() const;

private:
	std::deque<Particle> particles_;
	const sf::Texture& texture_;
	Particle::Type type_;

	mutable sf::VertexArray vertexArray_;
	mutable bool needsVertexUpdate_;
};

