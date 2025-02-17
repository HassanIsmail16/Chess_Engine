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
		sf::Vector2f windowPos = geometry.getEntryPosition(i);

		sf::Vector2f maskPos(
			windowPos.x - offsetX,
			windowPos.y - offsetY
		);

		renderEntry(moves[i], maskPos, i);
	}
	mask->display();

	sf::Sprite mask_sprite(mask->getTexture());
	mask_sprite.setPosition(
		offsetX,
		offsetY
	);

	window.draw(mask_sprite);
}

void MoveHistory::handleInput(const InputManager& input_manager) {
	if (!geometry.isInsideBody(input_manager.getMousePosition())) {
		return;
	}

	float scroll_amount = input_manager.getScrollDelta() * 0.002f;
	if (scroll_amount != 0.0f) {
		scroll_percent = std::clamp(scroll_percent - scroll_amount, 0.0f, 1.0f);
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
	
	float entry_number_font_size = geometry.getEntryWidth() * 0.14f;
	sf::Color entry_number_font_color(194, 127, 89);

	std::string entry_number = std::to_string((index / 2) + 1);
	
	float offsetX = geometry.getEntryAreaX() - geometry.getBodyWidth() * 0.1287f;
	float right_shift = geometry.getBodyWidth() * 0.042f;
	
	float right_shift_factor;
	if (entry_number.size() == 1) {
		right_shift_factor = 1;
	} else if (entry_number.size() == 2) {
		right_shift_factor = 0.5;
	} else {
		right_shift_factor = 0;
	}

	float entry_number_x = (geometry.getBodyX() + geometry.getBodyWidth() * 0.06f) - offsetX + right_shift_factor * right_shift;

	sf::Text entry_number_text(entry_number, font, entry_number_font_size);
	entry_number_text.setColor(entry_number_font_color);
	entry_number_text.setPosition(entry_number_x, entry_text_y);

	mask->draw(entry_number_text);
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
		entry_area_bounds.y
	);
}

void MoveHistory::recordCastlingMove(const Move& move, const std::string& hash, const std::string& algebraic_notation) {
	bool is_king_side = move.getType() == PositionType::KingSideCastle;
	Position rook_from(move.from.row, (is_king_side ? 7 : 0));
	Position rook_to(move.from.row, (is_king_side ? 5 : 3));
	Move rook_move(rook_from, rook_to);
	recordMove(MoveEntry(move, rook_move, hash, algebraic_notation));
}

std::string MoveHistory::formatMove(Move move) const {
	std::string result;

	result += static_cast<char>('a' + move.to.col);
	result += std::to_string(move.to.row + 1);

	return result;
}