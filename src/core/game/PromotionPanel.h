#pragma once
#include <SFML/Graphics.hpp>
#include "BoardUtilities.h"
#include "../ui/Button.h"

class PromotionPanel {
public:
	PromotionPanel(const Move& move);

	void render(sf::RenderWindow& window);
	void update(const float& dt);
	void handleInput(const InputManager& input_manager);

	
private:
	void updateButtonGeometry();

	sf::Sprite sprite;
	Position pawn_position;
	std::vector<Button> buttons;

	int width;
	int height;
	float x;
	float y;
};