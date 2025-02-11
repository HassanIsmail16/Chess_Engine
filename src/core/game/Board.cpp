#include "Board.h"
#include "../log/Logger.hpp"
#include "Piece.h"
#include "../managers/AssetManager.h"

Board::Board() {
	selected_piece = nullptr;
	last_move = nullptr;
	this->initializeBoard();
}

Board::Board(const Board& other) {
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			board[row][col] = (other.board[row][col] ? other.board[row][col]->clone() : nullptr);
		}
	}

	for (const auto& piece : other.white_captured) {
		white_captured.emplace_back(piece->clone());
	}

	for (const auto& piece : other.black_captured) {
		black_captured.emplace_back(piece->clone());
	}

	selected_piece = nullptr;
	last_move = nullptr;

	if (other.selected_piece) {
		Position position = other.selected_piece->getPosition();
		selected_piece = board[position.row][position.col].get();
	}

	if (other.last_move) {
		Position from = other.last_move->from;
		Position to = other.last_move->to;
		Piece* taken_over = nullptr;

		if (other.last_move->taken_over) {
			taken_over = board[to.row][to.col].get();
		}

		last_move = new Move(from, to, taken_over);
	}

	valid_moves = other.valid_moves;
	is_white_side = other.is_white_side;
}

void Board::update(const float& dt) {
	updateTileStates(dt);
}

void Board::render(sf::RenderWindow& window) {
	geometry.update(window);
	renderBoard(window);
	renderTiles(window);
}

void Board::makeMove(const Move& move) {
	auto& moving_piece = getPieceAt(move.from);
	selected_piece = nullptr;
	valid_moves.clear();
	setPieceAt(move.to, std::move(moving_piece));
}

void Board::undoLastMove() {
	
}

std::unique_ptr<Piece>& Board::getPieceAt(const Position& position) {
	if (!isInBounds(position)) {
		LOG_ERROR("Attempted to get piece at row: ", position.row, ", col: ", position.col, ", which is out of bounds");
		throw std::runtime_error("Out of Bounds");
	}
 
	return board[position.row][position.col];
}

void Board::setPieceAt(const Position& position, std::unique_ptr<Piece> new_piece) {
	if (!isInBounds(position)) {
		LOG_ERROR("Attempted to set piece at row: ", position.row, ", col: ", position.col, ", which is out of bounds");
		throw std::runtime_error("Out of Bounds");
	}

	if (!new_piece) {
		LOG_DEBUG("Setting piece at row: ", position.row, ", col: ", position.col, " to a nullptr");
	}

	board[position.row][position.col] = std::move(new_piece);
}

bool Board::isKingChecked(const ChessColor& color, bool skipMoveValidation) {
	auto king_position = getKingPosition(color);

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			auto& current_piece = getPieceAt(Position(row, col));

			if (!current_piece || current_piece->getColor() == color) {
				continue;
			}

			auto possible_moves = current_piece->getPossibleMoves();

			// Skip full move validation when checking if a move would expose the king
			if (skipMoveValidation) {
				auto attack = std::find(possible_moves.begin(), possible_moves.end(), king_position);
				if (attack != possible_moves.end() && !isPathObstructed(current_piece->getPosition(), king_position)) {
					return true;
				}
			} else {
				auto valid_moves = getValidMoves(possible_moves, current_piece.get());
				auto attack = std::find(valid_moves.begin(), valid_moves.end(), king_position);
				if (attack != valid_moves.end()) {
					return true;
				}
			}
		}
	}
	return false;
}

std::vector<Position> Board::getValidMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece) {
	if (!moving_piece) {
		return std::vector<Position>{};
	}

	std::vector<Position> valid_moves;
	ChessColor moving_piece_color = moving_piece->getColor();
	PieceType moving_piece_type = moving_piece->getType();
	Position moving_piece_position = moving_piece->getPosition();

	if (moving_piece_type == PieceType::Pawn) {
		return getValidatedPawnMoves(candidate_moves, moving_piece);
	}

	for (const auto& candidate_move : candidate_moves) {
		if (!isInBounds(candidate_move) ||
			hasFriendlyPiece(candidate_move, moving_piece_color) ||
			willCaptureKing(candidate_move)) {
			continue;
		}

		if (moving_piece->canBeObstructed() && isPathObstructed(moving_piece_position, candidate_move)) {
			continue;
		}

		// Create a temporary board copy to test if this move would expose the king
		Board temp_board(*this);
		Move temp_move(moving_piece_position, candidate_move, nullptr);
		temp_board.makeMove(temp_move);
		if (temp_board.isKingChecked(moving_piece_color, true)) { // Use skipMoveValidation=true to prevent recursion
			continue;
		}

		valid_moves.emplace_back(candidate_move);
	}

	return valid_moves;
}

std::vector<Position> Board::getValidatedPawnMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece) {
	if (!moving_piece) {
		return std::vector<Position>{};
	}

	std::vector<Position> valid_pawn_moves;
	ChessColor moving_piece_color = moving_piece->getColor();
	ChessColor opponent_color = (moving_piece_color == ChessColor::White ? ChessColor::Black : ChessColor::White);
	Position moving_piece_position = moving_piece->getPosition();

	Position single_step = candidate_moves[0];
	Position double_step = candidate_moves[1];
	Position right_capture = candidate_moves[2];
	Position left_capture = candidate_moves[3];

	// Single step forward
	if (isInBounds(single_step) && !hasPieceAt(single_step)) {
		valid_pawn_moves.emplace_back(single_step);
	}

	// Double step forward
	if (isInBounds(single_step) && isInBounds(double_step) &&
		!hasPieceAt(single_step) && !hasPieceAt(double_step) &&
		!moving_piece->hasMoved()) {
		valid_pawn_moves.emplace_back(double_step);
	}

	// Regular captures
	if (isInBounds(right_capture) && hasPieceAt(right_capture) &&
		getPieceAt(right_capture)->getColor() == opponent_color) {
		valid_pawn_moves.emplace_back(right_capture);
	}

	if (isInBounds(left_capture) && hasPieceAt(left_capture) &&
		getPieceAt(left_capture)->getColor() == opponent_color) {
		valid_pawn_moves.emplace_back(left_capture);
	}

	// En Passant
	if (last_move) {
		auto& last_moved_piece = getPieceAt(last_move->to);
		if (last_moved_piece &&
			last_moved_piece->getType() == PieceType::Pawn &&
			last_moved_piece->getColor() == opponent_color) {

			int vertical_distance = abs(last_move->from.row - last_move->to.row);
			int en_passant_row = (moving_piece_color == ChessColor::White ? 4 : 3);
			int col_distance = abs(last_moved_piece->getPosition().col - moving_piece_position.col);

			if (vertical_distance == 2 &&
				last_moved_piece->getPosition().row == en_passant_row &&
				col_distance == 1) {

				int direction = (moving_piece_color == ChessColor::White ? 1 : -1);
				Position en_passant_move(moving_piece_position.row + direction,
					last_moved_piece->getPosition().col,
					PositionType::EnPassant);

				if (isInBounds(en_passant_move)) {
					valid_pawn_moves.emplace_back(en_passant_move);
				}
			}
		}
	}

	// Filter out moves that would expose the king
	std::vector<Position> final_valid_moves;
	for (const auto& move : valid_pawn_moves) {
		Board temp_board(*this);
		Move temp_move(moving_piece_position, move, nullptr);
		temp_board.makeMove(temp_move);
		if (!temp_board.isKingChecked(moving_piece_color, true)) {
			final_valid_moves.emplace_back(move);
		}
	}

	return final_valid_moves;
}

const std::vector<std::unique_ptr<Piece>>& Board::getCapturedPieces(const ChessColor& color) const {
	return (color == ChessColor::White ? white_captured : black_captured);
}

void Board::selectPiece(const Position& position) {
	selected_piece = getPieceAt(position).get();
	if (!selected_piece) {
		selected_piece = nullptr;
		valid_moves.clear();
		return;
	}
	LOG_INFO("A Piece of code ", selected_piece->getPieceCode(), " was selected");
	valid_moves = getValidMoves(selected_piece->getPossibleMoves(), selected_piece);
}

void Board::unselectPiece() {
	selected_piece = nullptr;
	valid_moves.clear();
}

bool Board::isWhiteSide() const {
	return is_white_side;
}

void Board::flip() {
	is_white_side = false;
}

uint64_t Board::computeHash() const {
	return 0;
}

BoardGeometry& Board::getGeometry() {
	return geometry;
}

void Board::initializeBoard() {
	// white pieces
	board[0][0] = std::make_unique<Piece>(PieceType::Rook, ChessColor::White, Position(0, 0));
	board[0][1] = std::make_unique<Piece>(PieceType::Knight, ChessColor::White, Position(0, 1));
	board[0][2] = std::make_unique<Piece>(PieceType::Bishop, ChessColor::White, Position(0, 2));
	board[0][3] = std::make_unique<Piece>(PieceType::Queen, ChessColor::White, Position(0, 3));
	board[0][4] = std::make_unique<Piece>(PieceType::King, ChessColor::White, Position(0, 4));
	board[0][5] = std::make_unique<Piece>(PieceType::Bishop, ChessColor::White, Position(0, 5));
	board[0][6] = std::make_unique<Piece>(PieceType::Knight, ChessColor::White, Position(0, 6));
	board[0][7] = std::make_unique<Piece>(PieceType::Rook, ChessColor::White, Position(0, 7));

	// white pawns
	for (int i = 0; i < 8; i++) {
		board[1][i] = std::make_unique<Piece>(PieceType::Pawn, ChessColor::White, Position(1, i));
	}

	// black pieces;
	board[7][0] = std::make_unique<Piece>(PieceType::Rook, ChessColor::Black, Position(7, 0));
	board[7][1] = std::make_unique<Piece>(PieceType::Knight, ChessColor::Black, Position(7, 1));
	board[7][2] = std::make_unique<Piece>(PieceType::Bishop, ChessColor::Black, Position(7, 2));
	board[7][3] = std::make_unique<Piece>(PieceType::Queen, ChessColor::Black, Position(7, 3));
	board[7][4] = std::make_unique<Piece>(PieceType::King, ChessColor::Black, Position(7, 4));
	board[7][5] = std::make_unique<Piece>(PieceType::Bishop, ChessColor::Black, Position(7, 5));
	board[7][6] = std::make_unique<Piece>(PieceType::Knight, ChessColor::Black, Position(7, 6));
	board[7][7] = std::make_unique<Piece>(PieceType::Rook, ChessColor::Black, Position(7, 7));
	
	// black pawns
	for (int i = 0; i < 8; i++) {
		board[6][i] = std::make_unique<Piece>(PieceType::Pawn, ChessColor::Black, Position(6, i));
	}

	updateTileStates(0);
}

void Board::renderBoard(sf::RenderWindow& window) {
	sf::Sprite board_frame = AssetManager::getInstance().getSprite("board-frame");
	board_frame.setScale(geometry.getBoardScale(), geometry.getBoardScale());
	board_frame.setPosition(geometry.getBoardX(), geometry.getMargin());
	window.draw(board_frame);
}

void Board::renderTiles(sf::RenderWindow& window) {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			renderTileAt(window, Position(row, col));
			renderPieceAt(window, Position(row, col));
		}
	}
}

void Board::renderTileAt(sf::RenderWindow& window, const Position& position) {
	std::string sprite_name = ((position.row + position.col) % 2 == 0 ? "even-square" : "odd-square");
	std::string overlay_name = getTileOverlayName(position);
	
	sf::Sprite tile_sprite = AssetManager::getInstance().getSprite(sprite_name);
	sf::Sprite overlay_sprite;
	if (!overlay_name.empty()) {
		overlay_sprite = AssetManager::getInstance().getSprite(overlay_name);
	}

	float tile_scale = geometry.getTileSize() / AssetManager::getInstance().getTexture(sprite_name).getSize().x;
	sf::Vector2f tile_position = geometry.getTilePosition(position);

	tile_sprite.setScale(tile_scale, tile_scale);
	overlay_sprite.setScale(tile_scale, tile_scale);

	tile_sprite.setPosition(tile_position);
	overlay_sprite.setPosition(tile_position);

	window.draw(tile_sprite);
	window.draw(overlay_sprite);
}

void Board::renderPieceAt(sf::RenderWindow& window, const Position& position) {
	auto& piece = getPieceAt(position);

	if (!piece) {
		return;
	} // skip if position is empty

	piece->render(window, geometry);
}

void Board::updateTileStates(const float& dt) {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			tile_states[row][col] = computeTileState(Position(row, col));
		}
	}
}

std::string Board::getTileOverlayName(const Position& position) {
	switch (tile_states[position.row][position.col]) {
		case TileState::Selected: return "selected-square";
		case TileState::Highlighted: return "highlighted-square";
		case TileState::Attacked: return "attacked-square";
		case TileState::LastMove: return "last-move-square";
		case TileState::Special: return "special-square";
		case TileState::None: default: return "";
	}
}

TileState Board::computeTileState(const Position& position) {
	if (selected_piece && selected_piece->getPosition() == position) {
		return TileState::Selected;
	} // handle selected piece

	if ((last_move) && (last_move->from == position || last_move->to == position)) {
		return TileState::LastMove;
	} // handle last move

	// TODO: handle king attacked

	auto find_position = std::find(valid_moves.begin(), valid_moves.end(), position);

	if (find_position != valid_moves.end()) {
		auto& current_piece = getPieceAt(position);

		if (current_piece) {
			return TileState::Attacked;
		}

		// TODO: handle special moves

		return TileState::Highlighted;
	}

	return TileState::None;
}

bool Board::isInBounds(const Position& position) const {
	return position.row < 8 && position.row >= 0 && position.col < 8 && position.col >= 0;
}

bool Board::hasFriendlyPiece(const Position& position, const ChessColor& color) {
	if (!hasPieceAt(position)) {
		return false;
	}

	return color == getPieceAt(position)->getColor();
}

bool Board::willCaptureKing(const Position& position) {
	if (!hasPieceAt(position)) {
		return false;
	}

	return getPieceAt(position)->getType() == PieceType::King;
}

bool Board::isPathObstructed(const Position& from, const Position& to) {
	int horizontal_distance = to.col - from.col;
	int vertical_distance = to.row - from.row;

	// Turn to {-1, 0, 1}
	int dx = (horizontal_distance != 0 ? horizontal_distance / abs(horizontal_distance) : 0);
	int dy = (vertical_distance != 0 ? vertical_distance / abs(vertical_distance) : 0);

	// Chech if there is a piece in between
	int steps = std::max(abs(horizontal_distance), abs(vertical_distance));
	int current_row, current_col;
	for (int step = 1; step < steps; ++step) {
		current_row = from.row + (dy * step);
		current_col = from.col + (dx * step);
		Position current_position = { current_row, current_col };
		if (hasPieceAt(current_position)) {
			return true;
		}
	}

	return false;
}

bool Board::hasPieceAt(const Position& position) {
	auto& piece = getPieceAt(position);
	return piece != nullptr;
}

bool Board::willExposeKing(const Position& from, const Position& to, const ChessColor& color) {
	Board copy(*this);
	Move move(from, to, nullptr);
	copy.makeMove(move);
	return copy.isKingChecked(color);
}


Position Board::getKingPosition(const ChessColor& color) {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			auto& current_piece = getPieceAt(Position(row, col));

			if (!current_piece || current_piece->getColor() != color || current_piece->getType() != PieceType::King) {
				continue;
			}

			return Position(row, col);
		}
	}

	LOG_ERROR(__FUNCTION__, " was invoked and no king was found");
	throw std::runtime_error("No King");
}
