#include "MoveHistory.h"
#include "../log/Logger.hpp"
#include "../events/EventDispatcher.h"

MoveHistory::MoveHistory() {
	reset();
}

void MoveHistory::recordMove(const Move& move, const std::string& hash, const std::string& algebraic_notation) {
	if (move.isCastling()) {
		recordCastlingMove(move, hash, algebraic_notation);
		return;
	}

	recordMove(MoveEntry(move, hash, algebraic_notation));
}

void MoveHistory::recordMove(const MoveEntry& entry) {
	if (current_index < (int) moves.size() - 1) {
		LOG_INFO("Truncating move history vector, current_index: ", current_index, ", history size: ", moves.size());
		moves.erase(moves.begin() + current_index + 1, moves.end());
	}

	moves.push_back(entry);
	current_index++;

	if (current_index > 12) {
		scroll_sync_pending = true;
	}
}

bool MoveHistory::canUndo() {
	return current_index > 0;
}

bool MoveHistory::canRedo() {
	return current_index < moves.size() - 1;
}

bool MoveHistory::canJumpTo(const int& index) {
	return index >= 0 && index < moves.size();
}

void MoveHistory::undo() {
	jumpToPreviousMove();
	moves.pop_back();
}

void MoveHistory::redo() {
	jumpToNextMove();
}

void MoveHistory::jumpToMove(const int& index) {
	if (index < 0 || index >= moves.size()) {
		LOG_ERROR("Tried to jump to move with index ", index, ", number of recorded moves: ", moves.size());
		throw std::runtime_error("Invalid Move History Jump");
	}

	LOG_DEBUG("Jumping to move: ", index, ", numbe of recorded moves: ", moves.size());
	current_index = index;
}

void MoveHistory::jumpToPreviousMove() {
	jumpToMove(current_index - 1);
}

void MoveHistory::jumpToNextMove() {
	jumpToMove(current_index + 1);
}

void MoveHistory::reset() {
	current_index = 0;
	moves.clear();
	view = nullptr;
	mask = nullptr;
}

int MoveHistory::getCurrentMoveIndex() {
	return current_index;
}

int MoveHistory::getTotalMoves() {
	return moves.size();
}

bool MoveHistory::isEmpty() const {
	return moves.empty();
}

const std::vector<MoveEntry>& MoveHistory::getMoveHistory() const {
	return moves;
}

const Move& MoveHistory::getLastMove() const {
	if (moves.empty()) {
		LOG_ERROR("Attempted to get last move on an empty move history vector")
		throw std::runtime_error("Invalid Move History Acesss");
	}

	return moves.back().move;
}

MoveHistoryGeometry MoveHistory::getGeometry() const {
	return geometry;
}

void MoveHistory::render(sf::RenderWindow& window) {
	geometry.update(window, scroll_percent, moves.size());

	if (!view || !mask) {
		initializeView(window);
	}

	sf::Sprite body = AssetManager::getInstance().getSprite("move-history");
	body.setScale(geometry.getBodyScale());
	body.setPosition(geometry.getBodyX(), geometry.getBodyY());
	window.draw(body);

	mask->clear(sf::Color::Transparent);

	float offsetX = geometry.getEntryAreaX() - geometry.getBodyWidth() * 0.1287f;
	float offsetY = geometry.getEntryAreaY();

	for (int i = 0; i < moves.size(); i++) {
		sf::Vector2f window_position = geometry.getEntryPosition(i);

		if (!geometry.isInsideBody(window_position)) {
			continue;
		}

		sf::Vector2f mas_position(
			window_position.x - offsetX,
			window_position.y - offsetY
		);

		renderEntry(moves[i], mas_position, i);
	}
	mask->display();

	sf::Sprite mask_sprite(mask->getTexture());
	mask_sprite.setPosition(
		offsetX,
		offsetY
	);

	window.draw(mask_sprite);

	renderScrollBar(window);
}

void MoveHistory::handleInput(const InputManager& input_manager) {
	if (!geometry.isInsideBody(input_manager.getMousePosition())) {
		return;
	}

	float scroll_amount = input_manager.getScrollDelta() * 0.005f;
	if (scroll_amount != 0.0f) {
		scroll_percent = std::clamp(scroll_percent - scroll_amount, 0.0f, geometry.getMaxScrollPercent());
	}

	if (input_manager.isMouseButtonJustPressed(sf::Mouse::Left)) {
		if (geometry.isInsideEntryRegion(input_manager.getMousePosition())) {
			int clicked_index = geometry.getEntryIndex(input_manager.getMousePosition());
			if (clicked_index >= 0 && clicked_index < moves.size()) {
				try {
					jumpToMove(clicked_index);
					
					EventDispatcher::getInstance().pushEvent(
						std::make_shared<JumpToMoveEvent>(clicked_index, moves[clicked_index].board_hash)
					);

				} catch (const std::runtime_error& e) {
					LOG_ERROR("Failed to jump to move: ", e.what());
				}
			}
		}
	}
}

void MoveHistory::renderEntry(MoveEntry entry, const sf::Vector2f& position, int index) {
	std::string sprite_name = (index % 2 == 0 ? "blue-button" : "red-button");
	
	sf::Sprite sprite = AssetManager::getInstance().getSprite(sprite_name);
	sprite.setScale(geometry.getEntryScale());
	sprite.setPosition(position);

	mask->draw(sprite);

	sf::Font font = AssetManager::getInstance().getFont("main-font");
	float entry_text_font_size = geometry.getEntryWidth() * 0.1f;
	sf::Color entry_text_font_color = (index % 2 == 0 ? sf::Color(145, 190, 219) : sf::Color(219, 151, 145));
	
	sf::Text entry_text(entry.algebraic_notation, font, entry_text_font_size);
	auto entry_text_bounds = entry_text.getLocalBounds();
	auto sprite_bounds = sprite.getGlobalBounds();

	float entry_text_x = sprite_bounds.left + (sprite_bounds.width - entry_text_bounds.width) / 2.0f - entry_text_bounds.left;
	float entry_text_y = sprite_bounds.top + (sprite_bounds.height * 0.87 - entry_text_bounds.height) / 2.0f - entry_text_bounds.top;
	
	entry_text.setColor(entry_text_font_color);
	entry_text.setPosition(entry_text_x, entry_text_y);

	mask->draw(entry_text);
	
	float entry_number_font_size = geometry.getEntryWidth() * 0.16f;
	sf::Color entry_number_font_color(194, 127, 89);

	std::string entry_number = std::to_string((index / 2) + 1);
	
	if (index % 2) {
		return;
	}

	float offsetX = geometry.getEntryAreaX() - geometry.getBodyWidth() * 0.1287f;
	float right_shift = geometry.getBodyWidth() * 0.042f;
	
	float right_shift_factor;
	if (entry_number.size() == 1) {
		right_shift_factor = 1;
	} else if (entry_number.size() == 2) {
		right_shift_factor = 0.5;
	} else {
		right_shift_factor = 0.08;
	}

	float entry_number_x = (geometry.getBodyX() + geometry.getBodyWidth() * 0.06f) - offsetX + right_shift_factor * right_shift;

	sf::Text entry_number_text(entry_number, font, entry_number_font_size);
	entry_number_text.setColor(entry_number_font_color);
	entry_number_text.setPosition(entry_number_x, entry_text_y);

	mask->draw(entry_number_text);
}

void MoveHistory::renderScrollBar(sf::RenderWindow& window) {
	renderScrollBarArrows(window);
	renderScrollBarKnob(window);
}

void MoveHistory::renderScrollBarArrows(sf::RenderWindow& window) {
	float body_width = geometry.getBodyWidth();
	float body_height = geometry.getBodyHeight();

	float arrow_x = geometry.getBodyX() + 0.824f * body_width;
	float up_arrow_y = geometry.getBodyY() + 0.1f * body_height;
	float down_arrow_y = geometry.getBodyY() + body_height - (body_height * 0.197f);

	float arrow_width = body_width * 0.054;
	float arrow_height = body_height * 0.039f;
	
	sf::Sprite up_arrow_sprite = AssetManager::getInstance().getSprite("up-scroll-arrow");
	sf::Sprite down_arrow_sprite = AssetManager::getInstance().getSprite("down-scroll-arrow");

	sf::Vector2f arrow_scale(
		arrow_width / up_arrow_sprite.getTexture()->getSize().x,
		arrow_height / up_arrow_sprite.getTexture()->getSize().y
	);

	up_arrow_sprite.setScale(arrow_scale);
	down_arrow_sprite.setScale(arrow_scale);

	up_arrow_sprite.setPosition(arrow_x, up_arrow_y);
	down_arrow_sprite.setPosition(arrow_x, down_arrow_y);

	window.draw(up_arrow_sprite);
	window.draw(down_arrow_sprite);
}

void MoveHistory::renderScrollBarKnob(sf::RenderWindow& window) {
	if (scroll_sync_pending) {
		scroll_percent = geometry.getMaxScrollPercent();
		scroll_sync_pending = false;
	}

	float total_height = geometry.getTotalHeight();
	float visible_height = geometry.getVisibleHeight();
	float scroll_offset = geometry.getScrollOffset();
	
	float housing_height = geometry.getScrollHousingHeight();
	float min_knob_height = geometry.getMinKnobHeight();
	float max_knob_height = geometry.getMaxKnobHeight();
	float knob_height = geometry.getFullKnobHeight();
	float knob_x = geometry.getKnobX();
	float knob_y = geometry.getKnobY();

	float knob_width = geometry.getKnobWidth();
	float upper_part_height = geometry.getUpperKnobHeight();
	float lower_part_height = geometry.getLowerKnobHeight();
	float middle_part_height = geometry.getMiddleKnobHeight();

	sf::Sprite upper_part = AssetManager::getInstance().getSprite("knob-up");
	sf::Sprite lower_part = AssetManager::getInstance().getSprite("knob-down");
	sf::Sprite middle_part = AssetManager::getInstance().getSprite("knob-center");

	upper_part.setScale(
		knob_width / upper_part.getTexture()->getSize().x,
		upper_part_height / upper_part.getTexture()->getSize().y
	);

	middle_part.setScale(
		knob_width / middle_part.getTexture()->getSize().x,
		middle_part_height / middle_part.getTexture()->getSize().y
	);

	lower_part.setScale(
		knob_width / lower_part.getTexture()->getSize().x,
		lower_part_height / lower_part.getTexture()->getSize().y
	);

	upper_part.setPosition(knob_x, knob_y);
	knob_y += upper_part_height;
	middle_part.setPosition(knob_x, knob_y);
	knob_y += middle_part_height;
	lower_part.setPosition(knob_x, knob_y);

	window.draw(upper_part);
	window.draw(middle_part);
	window.draw(lower_part);

}

void MoveHistory::initializeView(sf::RenderWindow& window) {
	geometry.update(window, scroll_percent, moves.size());

	sf::Vector2f entry_area_bounds(
		geometry.getEntryAreaWidth(),
		geometry.getEntryAreaHeight()
	);

	float left_shift_x = geometry.getBodyWidth() * 0.1287f;

	// initialize view
	view = std::make_unique<sf::View>();
	view->setSize(
		sf::Vector2f(
			entry_area_bounds.x + left_shift_x,
			entry_area_bounds.y
		)
	);
	view->setCenter((left_shift_x + entry_area_bounds.x) / 2, entry_area_bounds.y / 2);

	// initialize mask
	mask = std::make_unique<sf::RenderTexture>();
	mask->create(
		entry_area_bounds.x + left_shift_x,
		entry_area_bounds.y + 0.009f * window.getSize().y
	);
}

void MoveHistory::recordCastlingMove(const Move& move, const std::string& hash, const std::string& algebraic_notation) {
	bool is_king_side = move.getType() == PositionType::KingSideCastle;
	Position rook_from(move.from.row, (is_king_side ? 7 : 0));
	Position rook_to(move.from.row, (is_king_side ? 5 : 3));
	Move rook_move(rook_from, rook_to);
	recordMove(MoveEntry(move, rook_move, hash, algebraic_notation));
}
