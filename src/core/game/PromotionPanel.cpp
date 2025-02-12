#include "PromotionPanel.h"
#include "../events/EventDispatcher.h"

PromotionPanel::PromotionPanel(const Move& move) {
	pawn_position = move.to;
	
	sf::Vector2f invalid_position = sf::Vector2f(-1, -1);

	auto push_promotion_event = [move](const PieceType& promotion_type) {
		EventDispatcher::getInstance().pushEvent(
			std::make_shared<PromotionEvent>(move, promotion_type)
		);
		};

	buttons.emplace_back(
		"queen-promotion-btn",
		invalid_position,
		invalid_position,
		[push_promotion_event]() { push_promotion_event(PieceType::Queen); }
	);

	buttons.emplace_back(
		"knight-promotion-btn",
		invalid_position,
		invalid_position,
		[push_promotion_event]() { push_promotion_event(PieceType::Knight); }
	);

	buttons.emplace_back(
		"rook-promotion-btn",
		invalid_position,
		invalid_position,
		[push_promotion_event]() { push_promotion_event(PieceType::Rook); }
	);

	buttons.emplace_back(
		"bishop-promotion-btn",
		invalid_position,
		invalid_position,
		[push_promotion_event]() { push_promotion_event(PieceType::Bishop); }
	);
}

void PromotionPanel::render(sf::RenderWindow& window) {
	sprite = AssetManager::getInstance().getSprite("promotion-panel-body");

	width = sprite.getTexture()->getSize().x;
	height = sprite.getTexture()->getSize().y;
	x = (window.getSize().x / 2) - (width / 2);
	y = (window.getSize().y / 2) - (height / 2);
	

	sprite.setPosition(x, y);
	window.draw(sprite);

	updateButtonGeometry();

	for (auto& button : buttons) {
		button.render(window);
	}
}

void PromotionPanel::update(const float& dt) {
	return;
}

void PromotionPanel::handleInput(const InputManager& input_manager) {
	for (auto& button : buttons) {
		button.handleInput(input_manager);
	}
}

void PromotionPanel::updateButtonGeometry() {
	float margin = width * 0.05f;
	
	float button_width = (width - (margin * 5)) / 4;
	float button_height = height * 0.75f;
	
	float button_x = x + margin;
	float button_y = y + 2 * margin;

	for (auto& button : buttons) {
		button.setPosition(sf::Vector2f(button_x, button_y));
		button.setSize(sf::Vector2f(button_width, button_height));
		button_x += button_width + margin;
	}
}

