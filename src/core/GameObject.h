#pragma once

#include <SFML/Graphics.hpp>

class GameObject {
public:
	virtual void update(const float& dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};