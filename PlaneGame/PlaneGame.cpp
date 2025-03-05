#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem>

#include "ResourceHolder.h"

// Resource ID for sf::Texture
namespace Textures
{
	enum class ID
	{
		Landscape,
		Airplane,
	};
}

int main()
{
	std::cout << "Hello, World!" << std::endl;
	std::cout << "Current path is: " << std::filesystem::current_path() << std::endl;
	sf::RenderWindow window(sf::VideoMode(800, 600), "Plane Game");	
	window.setFramerateLimit(60);

	// Try to load resources
	ResourceHolder<sf::Texture, Textures::ID> textures;
	try
	{
		textures.load(Textures::ID::Landscape, "Media/Textures/Desert.png");
		textures.load(Textures::ID::Airplane, "Media/Textures/Eagle.png");
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	// Access resources
	sf::Sprite landscape{textures.get(Textures::ID::Landscape)};
	sf::Sprite airplane{textures.get(Textures::ID::Airplane)};

	airplane.setPosition(200.f, 200.f);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
			{
				window.close();
				return 0;
			}

			window.clear();
			window.draw(landscape);
			window.draw(airplane);
			window.display();
		}
	}
}

