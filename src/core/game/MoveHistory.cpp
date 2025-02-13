#include "MoveHistory.h"
#include "../log/Logger.hpp"
#include "../events/EventDispatcher.h"

MoveHistory::MoveHistory() {
	reset();
}

void MoveHistory::recordMove(const Move& move, const std::string& hash) {
	if (move.isCastling()) {
		recordCastlingMove(move, hash);
		return;
	}

	recordMove(MoveEntry(move, hash));
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

	sf::Sprite panel = AssetManager::getInstance().getSprite("move-history-panel");
	panel.setPosition(geometry.getBodyX(), geometry.getBodyY());
	panel.setScale(geometry.getBodyScale());
	window.draw(panel);

	for (int i = 0; i < moves.size(); ++i) {
		sf::Vector2f position = geometry.getEntryPosition(i);
		if (position.x >= 0 && position.y >= 0) {
			renderEntry(window, moves[i], position);
		}
	}
}

void MoveHistory::handleInput(const InputManager& input_manager) {
	if (!geometry.isInsideBody(input_manager.getMousePosition())) {
		return;
	}

	float scroll_amount = input_manager.getScrollDelta() * 0.1f;
	if (scroll_amount != 0.0f) {
		scroll_percent = std::clamp(scroll_percent - scroll_amount, 0.0f, 1.0f);
	}

	if (input_manager.isMouseButtonJustPressed(sf::Mouse::Left)) {
		if (geometry.isInsideEntryRegion(input_manager.getMousePosition())) {
			int clicked_index = geometry.getEntryIndex(input_manager.getMousePosition());
			if (clicked_index >= 0 && clicked_index < moves.size()) {
				try {
					jumpToMove(clicked_index);
					
					LOG_INFO("Jumping to move: ", clicked_index, ", number of recorded moves: ", moves.size());

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

void MoveHistory::renderEntry(sf::RenderWindow& window, MoveEntry entry, const sf::Vector2f& position) {
	sf::Text move_text;
	move_text.setFont(AssetManager::getInstance().getFont("main-font"));
	move_text.setCharacterSize(static_cast<unsigned int>(geometry.getEntryHeight() * 0.6f));
	move_text.setFillColor(sf::Color::Black);

	std::string move_str = formatMove(entry.move);
	if (entry.isCastling()) {
		move_str = entry.move.to.type == PositionType::KingSideCastle ? "O-O" : "O-O-O";
	}
	move_text.setString(move_str);

	sf::FloatRect text_bounds = move_text.getLocalBounds();
	move_text.setPosition(
		position.x + (geometry.getEntryWidth() - text_bounds.width) / 2,
		position.y + (geometry.getEntryHeight() - text_bounds.height) / 2
	);

	if (moves.size() > current_index && &entry == &moves[current_index]) {
		sf::RectangleShape highlight;
		highlight.setSize(sf::Vector2f(geometry.getEntryWidth(), geometry.getEntryHeight()));
		highlight.setPosition(position);
		highlight.setFillColor(sf::Color(255, 255, 0, 128));
		window.draw(highlight);
	}

	window.draw(move_text);
}

void MoveHistory::recordCastlingMove(const Move& move, const std::string& hash) {
	bool is_king_side = move.getType() == PositionType::KingSideCastle;
	Position rook_from(move.from.row, (is_king_side ? 7 : 0));
	Position rook_to(move.from.row, (is_king_side ? 5 : 3));
	Move rook_move(rook_from, rook_to);
	recordMove(MoveEntry(move, rook_move, hash));
}

std::string MoveHistory::formatMove(Move move) const {
	std::string result;

	result += static_cast<char>('a' + move.to.col);
	result += std::to_string(move.to.row + 1);

	return result;
}