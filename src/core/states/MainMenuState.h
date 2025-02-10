#pragma once

#include "State.h"

class MainMenuState : public State {
public:
	void run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) override;
	void exit() override;
	std::string getName() const override;
	~MainMenuState() override = default;


private:
	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;
	void handleInput(const InputManager& input_manager) override;

};