#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Text.hpp>


#include "Label.h"

namespace GUI
{
	Label::Label(const std::string& text, const FontHolder& fonts)
		: text_(text, fonts.get(Fonts::ID::Main), 16)
	{
	}

	bool Label::isSelectable() const
	{
		return false;
	}

	void Label::handleEvent(const sf::Event& event)
	{
	}

	void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(text_, states);
	}

	void Label::setText(const std::string& text)
	{
		text_.setString(text);
	}
}
