#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include "Application.h"


int main()
{
	try
	{
		Application app;
		app.run();
	}
	catch (std::exception& e)
	{
		std::cout << "\nEXCEPTION: " << e.what() << std::endl;
	}
}

