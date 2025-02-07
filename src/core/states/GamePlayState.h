#pragma once

#include "State.h"

class GamePlayState : public State {
public:
	void run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) override;
	void exit() override;
	~GamePlayState() = default;

private:
	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;
	void handleInput(const InputManager& input_manager) override;
};