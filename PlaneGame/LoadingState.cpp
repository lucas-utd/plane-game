#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include "LoadingState.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "Utility.h"


LoadingState::LoadingState(StateStack& stack, Context context)
	: State(stack, context)
	, loadingText_()
	, progressBar_()
	, progressBarBackground_()
	, loadingTask_()
{
	sf::RenderWindow& window = *getContext().window;
	sf::Font& font = context.fonts->get(Fonts::ID::Main);
	sf::Vector2f viewSize = window.getView().getSize();

	// Set up the loading text
	loadingText_.setFont(font);
	loadingText_.setString("Loading Resource");
	centerOrigin(loadingText_);
	loadingText_.setPosition(viewSize.x / 2.f, viewSize.y / 2.f + 50.f);

	// Set up the progress bar background
	progressBarBackground_.setFillColor(sf::Color::White);
	progressBarBackground_.setSize(sf::Vector2f(viewSize.x - 20, 10.f));
	progressBarBackground_.setPosition(10, loadingText_.getPosition().y + 40.f);

	// Set up the progress bar
	progressBar_.setFillColor(sf::Color(100, 100, 100));
	progressBar_.setSize(sf::Vector2f(200, 10.f));
	progressBar_.setPosition(10, loadingText_.getPosition().y + 40.f);

	setCompletion(0.f);

	// Start the loading task
	loadingTask_.execute();
}

void LoadingState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(loadingText_);
	window.draw(progressBarBackground_);
	window.draw(progressBar_);
}

bool LoadingState::update(sf::Time dt)
{
	// Update the loading bar from the remote task or finish it
	if (loadingTask_.isFinished())
	{
		requestStackPop();
		requestStackPush(States::ID::Game);
	}
	else
	{
		setCompletion(loadingTask_.getCompletion());
	}
	return true;
}

bool LoadingState::handleEvent(const sf::Event& event)
{
	return true;
}

void LoadingState::setCompletion(float percent)
{
	if (percent > 1.f)
		percent = 1.f;

	progressBar_.setSize(sf::Vector2f(progressBarBackground_.getSize().x * percent, progressBar_.getSize().y));
}
