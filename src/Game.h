#pragma once
#include "StateManager.h"
#include "InputManager.h"

class Game {
public:
	Game();
	void run();

	~Game();

	const int FPS_CAP = 120;

private:
	void initializeAssets();
	void exit();

	sf::Clock dt_clock;

	sf::RenderWindow* window;
	StateManager* state_manager;
	InputManager* input_manager;

};