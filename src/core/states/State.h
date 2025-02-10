#pragma once
#include <SFML/Graphics.hpp>
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"

class State {
public:
	virtual void run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) = 0;
	virtual void exit() = 0;
	virtual std::string getName() const = 0;
	virtual ~State() = default;

private:
	virtual void update(const float& dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual void handleInput(const InputManager& input_manager) = 0;

};
