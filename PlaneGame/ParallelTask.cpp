#include "ParallelTask.h"

ParallelTask::ParallelTask()
	: thread_(&ParallelTask::runTask, this)
	, isFinished_(false)
{
}

void ParallelTask::execute()
{
	isFinished_ = false;
	elapsedTime_.restart();
	thread_.launch();
}

bool ParallelTask::isFinished() // Ensure return type is bool
{
	sf::Lock lock(mutex_);
	return isFinished_;
}

float ParallelTask::getCompletion()
{
	sf::Lock lock(mutex_);

	// 100% at 10 seconds of elapsed time
	return elapsedTime_.getElapsedTime().asSeconds() / 10.f;
}

void ParallelTask::runTask()
{
	// Dummy task - stall 10 seconds
	bool ended = false;
	while (!ended)
	{
		sf::Lock lock(mutex_);
		if (elapsedTime_.getElapsedTime().asSeconds() >= 10.f)
		{
			isFinished_ = true;
			ended = true;
		}
	}

	{
		// isFinished_ may be accessed from multiple threads, so we need to protect it
		sf::Lock lock(mutex_);
		isFinished_ = true;
	}
}
