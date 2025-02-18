#include "InputManager.h"
#include <iostream>

InputManager::InputManager() {
	initializeKeyStates();
	initializeMouseButtonStates();
	initializeMousePositions();
}

void InputManager::update(const sf::Event& event, const sf::RenderWindow& window) {
	// update key states
	if (event.type == sf::Event::KeyPressed) {
		current_key_states[event.key.code] = true; 
	} else if (event.type == sf::Event::KeyReleased) {
		current_key_states[event.key.code] = false;
	}

	// update mouse states
	if (event.type == sf::Event::MouseButtonPressed) {
		current_mouse_states[event.mouseButton.button] = true;
	} else if (event.type == sf::Event::MouseButtonReleased) {
		current_mouse_states[event.mouseButton.button] = false;
	}

	// update scroll delta
	if (event.type == sf::Event::MouseWheelScrolled) {
		if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
			scroll_delta = event.mouseWheelScroll.delta;
		}
	} else {
		scroll_delta = 0.0f;
	}

	// update mouse position
	current_mouse_position = sf::Mouse::getPosition(window);
}

void InputManager::endFrame() {
	previous_key_states = current_key_states;
	previous_mouse_states = current_mouse_states;
	previous_mouse_position = current_mouse_position;
	scroll_delta = 0.0f;
}

bool InputManager::isKeyJustPressed(const sf::Keyboard::Key& key) const {
	return current_key_states.at(key) && !previous_key_states.at(key);
}

bool InputManager::isKeyHeld(const sf::Keyboard::Key& key) const {
	return current_key_states.at(key) && previous_key_states.at(key);
}

bool InputManager::wasKeyReleased(const sf::Keyboard::Key& key) const {
	return !current_key_states.at(key) && previous_key_states.at(key);
}

bool InputManager::isMouseButtonJustPressed(const sf::Mouse::Button& button) const {
	return current_mouse_states.at(button) && !previous_mouse_states.at(button);
}

bool InputManager::isMouseButtonHeld(const sf::Mouse::Button& button) const {
	return current_mouse_states.at(button) && previous_mouse_states.at(button);
}

bool InputManager::wasMouseButtonReleased(const sf::Mouse::Button& button) const {
	return !current_mouse_states.at(button) && previous_mouse_states.at(button);
}

sf::Vector2i InputManager::getMousePosition() const {
	return current_mouse_position;
}

sf::Vector2i InputManager::getMouseDelta() const {
	return current_mouse_position - previous_mouse_position;
}

float InputManager::getScrollDelta() const {
	return scroll_delta;
}

bool InputManager::hasMouseMoved() const {
	return getMouseDelta() != sf::Vector2i(0, 0);
}

void InputManager::initializeKeyStates() {
	for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
		current_key_states[static_cast<sf::Keyboard::Key>(i)] = false;
	}
	previous_key_states = current_key_states;
}

void InputManager::initializeMouseButtonStates() {
	for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
		current_mouse_states[static_cast<sf::Mouse::Button>(i)] = false;
	}
	previous_mouse_states = current_mouse_states;
}

void InputManager::initializeMousePositions() {
	current_mouse_position = previous_mouse_position = sf::Vector2i(0, 0);
	scroll_delta = 0.0f;
	previous_scroll = 0.0f;
}
