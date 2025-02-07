#include "MainMenuState.h"
#include ../events/EventDispatcher.h"
#include "StateForward.h"

void MainMenuState::run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) {
	update(dt);
	handleInput(input_manager);
	render(window);
}

void MainMenuState::exit() {
	return;
}

void MainMenuState::update(const float& dt) {
	return;
}

void MainMenuState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::White);
	auto start_button = AssetManager::getInstance().getSprite("start-btn");
	start_button.setScale(sf::Vector2f(6, 6));
	start_button.setPosition(sf::Vector2f(300, 250));
	window.draw(start_button);
	window.display();
}

void MainMenuState::handleInput(const InputManager& input_manager) {
	// testing state transitions
	if (input_manager.isKeyJustPressed(sf::Keyboard::Key::Enter)) {
		EventDispatcher::getInstance().pushEvent(
			std::make_shared<ReplaceCurrentStateEvent>(
				std::make_unique<GamePlayState>()
			)
		);
	}
}
