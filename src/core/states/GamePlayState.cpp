#include "GamePlayState.h"
#include "../events/EventDispatcher.h"
#include "StateForward.h"

GamePlayState::GamePlayState() {
	board = std::make_unique<Board>();
}

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
	board->update(dt);
	return;
}

void GamePlayState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Green);
	board->render(window);
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

	if (input_manager.isMouseButtonJustPressed(sf::Mouse::Button::Left)) {
		Position selected_position = board->getGeometry().getTileBoardPosition(input_manager.getMousePosition());
		board->selectPiece(selected_position);
	}
}
