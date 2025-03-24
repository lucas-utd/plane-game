#include "Application.h"
#include "TitleState.h"
#include "MenuState.h"
#include "LoadingState.h"
#include "GameState.h"
#include "PauseState.h"
#include "StateIdentifiers.h"
#include "Utility.h"
#include "SettingsState.h"
#include "GameOverState.h"
#include "MultiplayerGameState.h"

const sf::Time Application::TimePerFrame = sf::seconds(1.f / 60.f);

Application::Application()
	: window_(sf::VideoMode(1024, 768), "Graphics", sf::Style::Close)
	, textures_()
	, fonts_()
	, music_()
	, sounds_()
	, keyBinding1_(1)
	, keyBinding2_(2)
	, stateStack_(State::Context(window_, textures_, fonts_, music_, sounds_, keyBinding1_, keyBinding2_))
	, statisticsText_()
	, statisticsUpdateTime_()
	, statisticsNumFrames_(0)
{
	window_.setKeyRepeatEnabled(false);
	window_.setVerticalSyncEnabled(true);

	fonts_.load(Fonts::Main, "Media/Sansation.ttf");

	textures_.load(Textures::TitleScreen, "Media/Textures/TitleScreen.png");
	textures_.load(Textures::Buttons, "Media/Textures/Buttons.png");

	statisticsText_.setFont(fonts_.get(Fonts::Main));
	statisticsText_.setPosition(5.f, 5.f);
	statisticsText_.setCharacterSize(10u);

	registerStates();
	stateStack_.pushState(States::Title);

	music_.setVolume(25.f);
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (window_.isOpen())
	{
		sf::Time dt = clock.restart();
		timeSinceLastUpdate += dt;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			// Check inside the loop, because stack might be empty before update() call
			if (stateStack_.isEmpty())
			{
				window_.close();
			}
		}

		updateStatistics(dt);
		render();
	}
}

void Application::processInput()
{
	sf::Event event;
	while (window_.pollEvent(event))
	{
		stateStack_.handleEvent(event);

		if (event.type == sf::Event::Closed)
		{
			window_.close();
		}
	}
}

void Application::update(sf::Time dt)
{
	stateStack_.update(dt);
}

void Application::render()
{
	window_.clear();

	stateStack_.draw();

	window_.setView(window_.getDefaultView());
	window_.draw(statisticsText_);

	window_.display();
}

void Application::updateStatistics(sf::Time dt)
{
	statisticsUpdateTime_ += dt;
	statisticsNumFrames_ += 1;
	if (statisticsUpdateTime_ >= sf::seconds(1.0f))
	{
		statisticsText_.setString("FPS: " + toString(statisticsNumFrames_));

		statisticsUpdateTime_ -= sf::seconds(1.0f);
		statisticsNumFrames_ = 0;
	}
}

void Application::registerStates()
{
	stateStack_.registerState<TitleState>(States::Title);
	stateStack_.registerState<MenuState>(States::Menu);
	stateStack_.registerState<GameState>(States::Game);
	stateStack_.registerState<MultiplayerGameState>(States::HostGame, true);
	stateStack_.registerState<MultiplayerGameState>(States::JoinGame, true);
	stateStack_.registerState<PauseState>(States::Pause);
	stateStack_.registerState<PauseState>(States::NetworkPause, true);
	stateStack_.registerState<SettingsState>(States::Settings);
	stateStack_.registerState<GameOverState>(States::GameOver, "Mission Failed!");
	stateStack_.registerState<GameOverState>(States::MissionSuccess, "Mission Successful!");
}
