#pragma once
#include <SFML/System/Thread.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Clock.hpp>

class ParallelTask
{
public:
	ParallelTask();

	void execute();
	bool isFinished(); // Change return type to bool
	float getCompletion();

private:
	void runTask();

private:
	sf::Thread thread_;
	bool isFinished_;
	sf::Clock elapsedTime_;
	sf::Mutex mutex_;
};

