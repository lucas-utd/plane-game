#pragma once
#include "Entity.h"


class Aircraft :
    public Entity
{
public:
	enum class Type
	{
		Eagle,
		Raptor,
	};

public:
	explicit Aircraft(Type type);
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Type type_;
	sf::Sprite sprite_;
};

