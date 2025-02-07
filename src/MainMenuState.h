#pragma once

#include "State.h"

class MainMenuState : public State {
public:
	void run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) override;
	void exit();
	~MainMenuState() = default;

private:
	void update(const float& dt);
	void render(sf::RenderWindow& window);
	void handleInput(const InputManager& input_manager);

};