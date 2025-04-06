#pragma once
#include <memory>
#include <vector>

#include "Component.h"


namespace GUI
{
	class Container : public Component
	{
	public:
		typedef std::shared_ptr<Container> Ptr;

	public:
		Container();

		void pack(Component::Ptr component);

		virtual bool isSelectable() const override;
		virtual void handleEvent(const sf::Event& event) override;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		bool hasSelection() const;
		void select(std::size_t index);
		void selectNext();
		void selectPrevious();

	private:
		std::vector<Component::Ptr> children_;
		int selectedChild_;
	};
}

