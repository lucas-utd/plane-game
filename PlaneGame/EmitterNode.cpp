#include "EmitterNode.h"
#include "ParticleNode.h"
#include "Command.h"
#include "CommandQueue.h"


EmitterNode::EmitterNode(Particle::Type type)
	: SceneNode()
	, accumulatedTime_()
	, type_(type)
	, particleSystem_(nullptr)
{
}

void EmitterNode::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (particleSystem_)
	{
		emitParticles(dt);
	}
	else
	{
		// Find particle node with the same type as emitter node
		auto finder = [this](ParticleNode& container, sf::Time)
			{
				if (container.getParticleType() == type_)
				{
					particleSystem_ = &container;
				}
			};

		Command command;
		command.category = Category::ParticleSystem;
		command.action = derivedAction<ParticleNode>(finder);

		commands.push(command);
	}
}

void EmitterNode::emitParticles(sf::Time dt)
{
	const float emissionRate = 30.f;
	const sf::Time interval = sf::seconds(1.f) / emissionRate;

	accumulatedTime_ += dt;

	while (accumulatedTime_ > interval)
	{
		accumulatedTime_ -= interval;
		particleSystem_->addParticle(getWorldPosition());
	}
}
