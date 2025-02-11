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

	// testing piece selections
	static int col = 0, row = 0;
	if (input_manager.isKeyJustPressed(sf::Keyboard::Right)) col++;
	if (input_manager.isKeyJustPressed(sf::Keyboard::Left)) col--;
	if (input_manager.isKeyJustPressed(sf::Keyboard::Up)) row--;
	if (input_manager.isKeyJustPressed(sf::Keyboard::Down)) row++;

	if (input_manager.isMouseButtonJustPressed(sf::Mouse::Button::Left)) {
		board->selectPiece(Position(row, col));
	}

	if (input_manager.isMouseButtonJustPressed(sf::Mouse::Button::Right)) {
		board->unselectPiece();
	}
}
