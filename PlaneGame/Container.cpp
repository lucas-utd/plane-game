#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


#include "Container.h"

namespace GUI
{
	Container::Container()
		: children_()
		, selectedChild_(-1)
	{
	}

	void Container::pack(Component::Ptr component)
	{
		children_.push_back(component);
		if (!hasSelection() && component->isSelectable())
		{
			select(children_.size() - 1);
		}
	}

	bool Container::isSelectable() const
	{
		return false;
	}

	void Container::handleEvent(const sf::Event& event)
	{
		// If we have a selected child, give it a chance to handle the event
		if (hasSelection() && children_[selectedChild_]->isActive())
		{
			children_[selectedChild_]->handleEvent(event);
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
			{
				selectPrevious();
			}
			else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
			{
				selectNext();
			}
			else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)
			{
				if (hasSelection())
				{
					children_[selectedChild_]->activate();
				}
			}
		}
	}

	void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();

		for (const auto& child : children_)
		{
			target.draw(*child, states);
		}
	}

	bool Container::hasSelection() const
	{
		return selectedChild_ >= 0;
	}

	void Container::select(std::size_t index)
	{
		if (children_[index]->isSelectable())
		{
			if (hasSelection())
			{
				children_[selectedChild_]->deselect();
			}
			children_[index]->select();
			selectedChild_ = index;
		}
	}

	void Container::selectNext()
	{
		if (!hasSelection())
		{
			return;
		}

		// Search next component that is selectable, wrap if necessary
		int next = selectedChild_;
		do
		{
			next = (next + 1) % children_.size();
		} while (!children_[next]->isSelectable());

		select(next);
	}

	void Container::selectPrevious()
	{
		if (!hasSelection())
		{
			return;
		}
		// Search previous component that is selectable, wrap if necessary
		int prev = selectedChild_;
		do
		{
			prev = (prev + children_.size() - 1) % children_.size();
		} while (!children_[prev]->isSelectable());

		select(prev);
	}
}
