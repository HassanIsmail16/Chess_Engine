#include "Button.h"
#include "../managers/AssetManager.h"

Button::Button(const std::string& name, const sf::Vector2f& position, const sf::Vector2f& size, std::function<void()> on_click) : name(name), position(position), size(size), on_click(on_click) {}

void Button::update(const float& dt) {
	return;
}

void Button::render(sf::RenderWindow& window) {
	sprite = AssetManager::getInstance().getSprite(name);
	sprite.setPosition(position);
	window.draw(sprite);
}

void Button::handleInput(const InputManager& input_manager) {
	if (isHovered(input_manager.getMousePosition()) && input_manager.isMouseButtonJustPressed(sf::Mouse::Button::Left)) {
		if (on_click) {
			on_click();
		}
	}
}

void Button::setOnClick(std::function<void()> on_click) {
	this->on_click = on_click;
}

void Button::setPosition(const sf::Vector2f& position) {
	this->position = position;
}

void Button::setSize(const sf::Vector2f& size) {
	this->size = size;
}

bool Button::isHovered(const sf::Vector2i& mouse_position) {
	return mouse_position.x > position.x && mouse_position.x < position.x + size.x &&
		mouse_position.y > position.y && mouse_position.y < position.y + size.y;
}

