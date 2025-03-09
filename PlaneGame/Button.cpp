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
		, normalTexture_(textures.get(Textures::ID::ButtonNormal))
		, selectedTexture_(textures.get(Textures::ID::ButtonSelected))
		, pressedTexture_(textures.get(Textures::ID::ButtonPressed))
		, sprite_()
		, text_("", fonts.get(Fonts::ID::Main), 16)
		, isToggle_(false)
	{
		sprite_.setTexture(normalTexture_);

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

		sprite_.setTexture(selectedTexture_);
	}

	void Button::deselect()
	{
		Component::deselect();

		sprite_.setTexture(normalTexture_);
	}

	void Button::activate()
	{
		Component::activate();

		// If we are toggle then we should show that the button is pressed and thus "toggled"
		if (isToggle_)
		{
			sprite_.setTexture(pressedTexture_);
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
				sprite_.setTexture(selectedTexture_);
			}
			else
			{
				sprite_.setTexture(normalTexture_);
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
}
