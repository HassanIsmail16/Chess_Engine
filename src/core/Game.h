#pragma once
#include "states/StateManager.h"
#include "managers/AssetManager.h"

class Game {
public:
	Game();

	void run();

	const int FPS_CAP = 120;

private:
	void initializeAssets();

	sf::Clock dt_clock;

	std::unique_ptr<sf::RenderWindow> window;
	std::unique_ptr<StateManager> state_manager;
	std::unique_ptr<InputManager> input_manager;
};