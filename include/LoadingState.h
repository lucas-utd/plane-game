#pragma once
#include <SFML/Graphics.hpp>

#include "State.h"
#include "ParallelTask.h"
class LoadingState : public State
{
public:
	LoadingState(StateStack& stack, Context context);

	virtual void draw() override;
	virtual bool update(sf::Time dt) override;
	virtual bool handleEvent(const sf::Event& event) override;

	void setCompletion(float completion);

private:
	sf::Text loadingText_;
	sf::RectangleShape progressBar_;
	sf::RectangleShape progressBarBackground_;

	ParallelTask loadingTask_;
};

