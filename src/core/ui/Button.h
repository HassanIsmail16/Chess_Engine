#pragma once
#include <SFML/Graphics.hpp>
#include "../managers/InputManager.h"
#include <functional>

class Button {
public:
	Button(const std::string& name, const sf::Vector2f& position, const sf::Vector2f& size, std::function<void()> on_click = nullptr);

	void update(const float& dt);
	void render(sf::RenderWindow& window);
	void handleInput(const InputManager& input_manager);

	void setOnClick(std::function<void()> on_click);
	void setPosition(const sf::Vector2f& position);
	void setSize(const sf::Vector2f& size);

	bool isHovered(const sf::Vector2i& mouse_position);

private:
	sf::Sprite sprite;
	std::string name;
	
	sf::Vector2f position;
	sf::Vector2f size;

	std::function<void()> on_click;
};