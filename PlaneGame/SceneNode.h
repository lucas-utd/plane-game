#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Command.h"

class SceneNode : public sf::Transformable, public sf::Drawable,
	private sf::NonCopyable
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;

public:
	SceneNode();

	void attachChild(Ptr child);
	Ptr detachChild(const SceneNode& node);

	void update(sf::Time dt);

	sf::Vector2f getWorldPosition() const;
	sf::Transform getWorldTransform() const;

	void onCommand(const Command& command, sf::Time dt);
	virtual unsigned int getCategory() const;

private:
	virtual void updateCurrent(sf::Time dt);
	void updateChildren(sf::Time dt);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;


private:
	std::vector<Ptr> children_;
	SceneNode* parent_;
};

