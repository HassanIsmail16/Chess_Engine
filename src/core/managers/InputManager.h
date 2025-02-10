#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

class InputManager {
public:
	InputManager();

	void update(const sf::Event& event, const sf::RenderWindow& window);
	void endFrame();

	bool isKeyJustPressed(const sf::Keyboard::Key& key) const;
	bool isKeyHeld(const sf::Keyboard::Key& key) const;
	bool wasKeyReleased(const sf::Keyboard::Key& key) const;

	bool isMouseButtonJustPressed(const sf::Mouse::Button& button) const;
	bool isMouseButtonHeld(const sf::Mouse::Button& button) const;
	bool wasMouseButtonReleased(const sf::Mouse::Button& button) const;

	sf::Vector2i getMousePosition() const;
	sf::Vector2i getMouseDelta() const;
	bool hasMouseMoved() const;

private:
	void initializeKeyStates();
	void initializeMouseButtonStates();
	void initializeMousePositions();

	// keyboard data
	std::unordered_map<sf::Keyboard::Key, bool> current_key_states;
	std::unordered_map<sf::Keyboard::Key, bool> previous_key_states;
	
	// mouse data
	std::unordered_map<sf::Mouse::Button, bool> current_mouse_states;
	std::unordered_map<sf::Mouse::Button, bool> previous_mouse_states;

	sf::Vector2i current_mouse_position;
	sf::Vector2i previous_mouse_position;
};
