#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Component.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"

namespace GUI
{
	class Button : public Component
	{
	public:
		typedef std::shared_ptr<Button> Ptr;
		typedef std::function<void()> Callback;

	public:
		Button(const FontHolder& fonts, const TextureHolder& textures);

		void setCallback(Callback callback);
		void setText(const std::string& text);
		void setToggle(bool flag);

		virtual bool isSelectable() const;
		virtual void select();
		virtual void deselect();

		virtual void activate();
		virtual void deactivate();

		virtual void handleEvent(const sf::Event& event);

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		Callback callback_;
		const sf::Texture& normalTexture_;
		const sf::Texture& selectedTexture_;
		const sf::Texture& pressedTexture_;
		sf::Sprite sprite_;
		sf::Text text_;
		bool isToggle_;
	};
}

