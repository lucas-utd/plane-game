#include <SFML/Window/Event.hpp>  
#include <SFML/Graphics/RenderTarget.hpp>  
#include <SFML/Graphics/RenderStates.hpp>  

#include "Button.h"  
#include "Utility.h"  
#include "ResourceIdentifiers.h" // Fixed typo from "#inlcude" to "#include"

namespace GUI
{
	Button::Button(const FontHolder& fonts, const TextureHolder& textures)
		: callback_()
		, sprite_(textures.get(Textures::Buttons))
		, text_("", fonts.get(Fonts::Main), 16)
		, isToggle_(false)
	{
		changeTexture(Normal);

		sf::FloatRect bounds = sprite_.getLocalBounds();
		text_.setPosition(bounds.width / 2.f, bounds.height / 2.f);
	}

	void Button::setCallback(Callback callback)
	{
		callback_ = std::move(callback);
	}

	void Button::setText(const std::string& text)
	{
		text_.setString(text);
		centerOrigin(text_);
	}

	void Button::setToggle(bool flag)
	{
		isToggle_ = flag;
	}

	bool Button::isSelectable() const
	{
		return true;
	}

	void Button::select()
	{
		Component::select();

		changeTexture(Selected);
	}

	void Button::deselect()
	{
		Component::deselect();

		changeTexture(Normal);
	}

	void Button::activate()
	{
		Component::activate();

		// If we are toggle then we should show that the button is pressed and thus "toggled"
		if (isToggle_)
		{
			changeTexture(Pressed);
		}

		if (callback_)
		{
			callback_();
		}

		// If we are not a toggle then deactivate the button since we are just momentarily activate
		if (!isToggle_)
		{
			deactivate();
		}
	}

	void Button::deactivate()
	{
		Component::deactivate();
		if (isToggle_)
		{
			// Reset texture to right one depending on if we are selected or not
			if (isSelected())
			{
				changeTexture(Selected);
			}
			else
			{
				changeTexture(Normal);
			}
		}
	}

	void Button::handleEvent(const sf::Event&)
	{
		// Nothing to do here since we are not interested in events
	}

	void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(sprite_, states);
		target.draw(text_, states);
	}

	void Button::changeTexture(Type buttonType)
	{
		sf::IntRect textureRect(0, 50 * buttonType, 200, 50);
		sprite_.setTextureRect(textureRect);
	}
}
