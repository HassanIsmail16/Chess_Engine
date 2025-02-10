#pragma once
#include "GameObject.h"

class UIComponent : GameObject {
public:
	UIComponent(const int& x, const int& y) : x(x), y(y) {}

	virtual void update(const float& dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;

	int getX() { return x; }
	int getY() { return y; }

private:
	int x, y;
};