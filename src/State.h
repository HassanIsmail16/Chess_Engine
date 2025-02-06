#pragma once
#include <SFML/Graphics.hpp>

class State {
public:
	virtual void run() = 0;
	virtual void update(const float& dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual void handleInput() = 0; // TODO: Pass InputHandler as a parameter when it's added
	virtual void exit() = 0;
	virtual ~State() = default;
};