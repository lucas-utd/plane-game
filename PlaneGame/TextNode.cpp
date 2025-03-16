#include <SFML/Graphics/RenderTarget.hpp>


#include "TextNode.h"
#include "Utility.h"


TextNode::TextNode(const FontHolder& fonts, const std::string& text)
{
	text_.setFont(fonts.get(Fonts::Main));
	text_.setCharacterSize(20);
	setString(text);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(text_, states);
}

void TextNode::setString(const std::string& text)
{
	text_.setString(text);
	centerOrigin(text_);
}
