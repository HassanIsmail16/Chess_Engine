#include "Game.h"

Game::Game() {
	// initialize window
	window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "Chess!");
	window->setFramerateLimit(FPS_CAP);

	// initialize state manager
	state_manager = new StateManager();
	state_manager->pushNewState(std::make_unique<MainMenuState>());

	// initialize input manager
	input_manager = new InputManager();
	
	// initialize assets
	initializeAssets();
}

void Game::run() {
	while (window->isOpen()) {
		float dt = dt_clock.restart().asSeconds(); // get dt
		sf::Event event;
		
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				exit();
			}

			input_manager->update(event, *window);
		}
		
		state_manager->getCurrentState()->run(dt, *window, *input_manager);
		EventDispatcher::getInstance().processEvents();
		input_manager->endFrame();
	}
}

Game::~Game() {
	exit();
}

void Game::initializeAssets() {
	auto& asset_manager = AssetManager::getInstance();
	asset_manager.loadAssets("assets/images/");
	asset_manager.loadAssets("assets/fonts/");
	asset_manager.loadAssets("assets/sounds/");
}

void Game::exit() {
	// destroy window
	if (window) {
		window->close();
		delete window;
	}

	// destroy state manager
	if (state_manager) {
		while (state_manager->getCurrentState()) {
			state_manager->removeCurrentState();
		}
		delete state_manager;
	}

	// destroy input manager
	if (input_manager) {
		delete input_manager;
	}
}
