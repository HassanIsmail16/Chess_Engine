#include "GamePlayState.h"
#include "../events/EventDispatcher.h"
#include "StateForward.h"

void GamePlayState::run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) {
	update(dt);
	handleInput(input_manager);
	render(window);
}

void GamePlayState::exit() {
	return;
}

std::string GamePlayState::getName() const {
	return "GamePlayState";
}

void GamePlayState::update(const float& dt) {
	return;
}

void GamePlayState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Green);
	window.draw(AssetManager::getInstance().getSprite("king-w"));
	window.display();
}

void GamePlayState::handleInput(const InputManager& input_manager) {
	// testing state transitions
	if (input_manager.isKeyJustPressed(sf::Keyboard::Key::Enter)) {
		EventDispatcher::getInstance().pushEvent(
			std::make_shared<ReplaceCurrentStateEvent>(
				std::make_unique<MainMenuState>()
			)
		);
	}
}
