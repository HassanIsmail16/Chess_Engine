#include "Board.h"
#include "../log/Logger.hpp"
#include "Piece.h"

Board::Board() {
	this->initializeBoard();
}

Board::Board(const Board& other) {
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			board[row][col] = other.board[row][col]->clone();
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
	return;
}

void Board::render(sf::RenderWindow& window) {
	return;
}

void Board::makeMove(const Move& move) {

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
		LOG_WARNING("Setting piece at row: ", position.row, ", col: ", position.col, " to a nullptr");
	}

	board[position.row][position.col] = std::move(new_piece);
}

bool Board::isKingChecked(const ChessColor& color) {
	auto king_position = getKingPosition(color);

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			auto& current_piece = getPieceAt(Position(row, col));
			
			if (!current_piece) {
				continue;
			} // skip if no piece in current position

			if (current_piece->getColor() == color) {
				continue;
			} // skip if current position has a friendly piece

			auto possible_moves = current_piece->getPossibleMoves();
			auto valid_moves = getValidMoves(possible_moves, current_piece.get());

			auto attack = std::find(valid_moves.begin(), valid_moves.end(), king_position);

			if (attack != valid_moves.end()) {
				return true;
			} // piece is attacking king
		}
	}

	return false;
}

const std::vector<std::unique_ptr<Piece>>& Board::getCapturedPieces(const ChessColor& color) const {
	return (color == ChessColor::White ? white_captured : black_captured);
}

void Board::selectPiece(const Position& position) {
	selected_piece = getPieceAt(position).get();
	if (selected_piece) {
		LOG_INFO("A Piece of code ", selected_piece->getPieceCode(), " was selected");
	}
}

void Board::unselectPiece() {
	selected_piece = nullptr;
}

bool Board::isWhiteSide() const {
	return is_white_side;
}

void Board::flip() {
	is_white_side = false;
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
	board[7][0] = std::make_unique<Piece>(PieceType::Rook, ChessColor::White, Position(7, 0));
	board[7][1] = std::make_unique<Piece>(PieceType::Knight, ChessColor::White, Position(7, 1));
	board[7][2] = std::make_unique<Piece>(PieceType::Bishop, ChessColor::White, Position(7, 2));
	board[7][3] = std::make_unique<Piece>(PieceType::Queen, ChessColor::White, Position(7, 3));
	board[7][4] = std::make_unique<Piece>(PieceType::King, ChessColor::White, Position(7, 4));
	board[7][5] = std::make_unique<Piece>(PieceType::Bishop, ChessColor::White, Position(7, 5));
	board[7][6] = std::make_unique<Piece>(PieceType::Knight, ChessColor::White, Position(7, 6));
	board[7][7] = std::make_unique<Piece>(PieceType::Rook, ChessColor::White, Position(7, 7));
	
	// black pawns
	for (int i = 0; i < 8; i++) {
		board[6][i] = std::make_unique<Piece>(PieceType::Pawn, ChessColor::White, Position(6, i));
	}
}

std::vector<Position> Board::getValidMoves(std::vector<Position>& candidate_moves, Piece* moving_piece) {
	if (!moving_piece) {
		return std::vector<Position>{};
	}

	std::vector<Position> valid_moves;
	ChessColor moving_piece_color = moving_piece->getColor();
	PieceType moving_piece_type = moving_piece->getType();
	Position moving_piece_position = moving_piece->getPosition();

	if (moving_piece_type == PieceType::Pawn) {
		valid_moves = getValidatedPawnMoves(candidate_moves, moving_piece);
		return valid_moves;
	} // Including En Passant

	for (const auto& candidate_move : candidate_moves) {
		if (!isInBounds(candidate_move) || 
			hasFriendlyPiece(candidate_move, moving_piece_color) || 
			willCaptureKing(candidate_move)) {
			continue;
		}

		// Check if their is a piece in the way of rooks, bishops or queens.
		if (moving_piece->canBeObstructed() && isPathObstructed(moving_piece_position, candidate_move)) {
			continue;
		}

		if (willExposeKing(moving_piece_position, candidate_move, moving_piece_color)) {
			continue;
		}

		valid_moves.emplace_back(candidate_move);
	}

	// TODO: if the piece is a king, check if it can be castled and add it to the valid moves

	return valid_moves;
}

std::vector<Position> Board::getValidatedPawnMoves(std::vector<Position>& candidate_moves, Piece* moving_piece) {
	if (!moving_piece) {
		return std::vector<Position>{};
	}

	std::vector<Position> valid_pawn_moves;
	ChessColor moving_piece_color = moving_piece->getColor();
	ChessColor opponent_color = (color == ChessColor::White ? ChessColor::Black : ChessColor::White);

	Position moving_piece_position = moving_piece->getPosition();
	Position single_step = candidate_moves[0];
	Position double_step = candidate_moves[1];
	Position right_capture = candidate_moves[2];
	Position left_capture = candidate_moves[3];

	if (!hasPieceAt(single_step)) {
		valid_moves.emplace_back(single_step);
	}

	if (!hasPieceAt(single_step) && !hasPieceAt(double_step) && !moving_piece->hasMoved()) {
		valid_moves.emplace_back(double_step);
	}

	if (hasPieceAt(right_capture)) {
		if (getPieceAt(right_capture])->getColor() == opponent_color) {
			valid_pawn_moves.emplace_back(right_capture);
		}
	}

	if (hasPieceAt(left_capture)) {
		if (getPieceAt(left_capture)->getColor() == opponent_color) {
			valid_pawn_moves.emplace_back(left_capture);
		}
	}

	// En Passant
	if (last_move) {
		auto& last_moved_piece = getPieceAt(last_move->to);
		ChessColor last_moved_piece_color = last_moved_piece->getColor();
		PieceType last_moved_piece_type = last_moved_piece->getType();
		Position last_moved_piece_position = last_moved_piece->getPosition();

		if (last_moved_piece_type != PieceType::Pawn || last_moved_piece_color != opponent_color) {
			return valid_moves; 
		} // The en passant capture must be performed on the turn immediately after the pawn being captured moves.

		int vertical_distance = abs(last_move->from.row - last_move->to.row);
		if (vertical_distance != 2) {
			return valid_moves;
		} // The captured pawn must have moved two squares in one move, landing right next to the capturing pawn.

		int en_passant_row = (color == ChessColor::White ? 4 : 3);
		if (last_moved_piece_position.row != en_passant_row) {
			return valid_moves;
		} // The en passante can only be done on this row

		int col_distance = abs(last_moved_piece_position.col - moving_piece_position.col);
		if (distance != 1) {
			return valid_moves;
		} // check if the captured pawn is directly next to the en passant pawn

		// Calculate en passant move
		int direction = (color == ChessColor::White ? 1 : -1);
		Position en_passant_move(moving_piece_position.row + direction, last_moved_piece_position.col, PositionType::EnPassant);

		if (isInBounds(en_passant_move)) {
			valid_pawn_moves.emplace_back(en_passant_move);
		}
	}

	return valid_pawn_moves;
}

bool Board::isInBounds(const Position& position) const {
	return position.row < 8 && position.row >= 0 && position.col < 8 && position.col >= 0;
}

bool Board::hasFriendlyPiece(const Position& position, const ChessColor& color) const {
	if (!hasPieceAt(position)) {
		return false;
	}

	return color == getPieceAt(position)->getColor();
}

bool Board::willCaptureKing(const Position& position) const {
	if (!hasPieceAt(position)) {
		return false;
	}

	return getPieceAt(position)->getType() == PieceType::King;
}

bool Board::isPathObstructed(const Position& from, const Position& to) const {
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

bool Board::hasPieceAt(const Position& position) const {
	auto& piece = getPieceAt(position);
	return piece != nullptr;
}

bool Board::willExposeKing(const Position& from, const Position& to, const ChessColor& color) const {
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
