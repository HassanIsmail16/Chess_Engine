#include "MainMenuState.h"

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
	if (input_manager.isKeyJustPressed(sf::Keyboard::Key::Enter)) {
		EventDispatcher::getInstance().pushEvent(std::make_shared<Event>(EventType::ExitMainMenuEvent));
		EventDispatcher::getInstance().pushEvent(std::make_shared<Event>(EventType::GameStartEvent));
	}
}
