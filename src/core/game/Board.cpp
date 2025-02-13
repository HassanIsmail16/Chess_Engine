#include "Board.h"
#include "../log/Logger.hpp"
#include "Piece.h"
#include "../managers/AssetManager.h"
#include "MoveHistory.h"
#include "../events/EventDispatcher.h"

Board::Board() {
	selected_piece = nullptr;
	last_move = nullptr;
	halfmove_clock = 0;
	this->initializeBoard();
}

Board::Board(const std::string& board_hash) {
	selected_piece = nullptr;
	last_move = nullptr;
	halfmove_clock = 0;
	this->initializeBoard();
	loadFromHash(board_hash);
}

Board::Board(const Board& other) {
	copy(other);
}

Board& Board::operator=(const Board& other) {
	if (this != &other) {
		clear();
		copy(other);
	}

	return *this;
}

Board::~Board() {
	clear();
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

	if (!moving_piece) {
		return;
	}

	if (move.isCastling()) {
		LOG_INFO("Applying castling move");
		makeCastlingMove(move);
	}

	moving_piece->setPosition(move.to);

	moving_piece->incrementMoveCount();
	
	selected_piece = nullptr;
	
	valid_moves.clear();
	
	setPieceAt(move.to, std::move(moving_piece));

	if (last_move) {
		delete last_move;
	}

	last_move = new Move(move.from, move.to, move.taken_over);

	if (getPieceAt(move.to)->getType() == PieceType::Pawn || move.taken_over) {
		halfmove_clock = 0;
	}
	else {
		halfmove_clock++;
	}

}

void Board::undoLastMove() {
	
}

void Board::promotePawn(const Position& position, const PieceType& promotion_type) {
	auto& pawn = getPieceAt(position);

	if (!pawn || pawn->getType() != PieceType::Pawn) {
		LOG_WARNING("Attempted to promote pawn at row: ", position.row, ", col: ", position.col, " which is not a pawn");
		return;
	}

	pawn = std::move(std::make_unique<Piece>(promotion_type, pawn->getColor(), position));
	LOG_INFO("The pawn at row: ", position.row, ", col: ", position.col, " was promoted to a ", pawn->getPieceCode());
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

			if (!current_piece) {
				continue;
			}

			if (current_piece->getColor() == color) {
				continue;
			}

			auto possible_moves = current_piece->getPossibleMoves();

			// Skip full move validation when checking if a move would expose the king
			if (skipMoveValidation) {
				auto attack = std::find(possible_moves.begin(), possible_moves.end(), king_position);
				if (attack == possible_moves.end()) {
					continue;
				}

				// handle pawn attacks	
				if (current_piece->getType() == PieceType::Pawn) {
					if (isValidPawnCapture(current_piece->getPosition(), king_position, current_piece->getColor())) {
						return true;
					} 
					continue;
				}
				
				// handle other attacks
				if (!isPathObstructed(current_piece->getPosition(), king_position)
					|| current_piece->getType() == PieceType::Knight) {
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
		return {};
	}

	std::vector<Position> valid_moves;
	ChessColor moving_piece_color = moving_piece->getColor();
	PieceType moving_piece_type = moving_piece->getType();
	Position moving_piece_position = moving_piece->getPosition();

	if (moving_piece_type == PieceType::Pawn) {
		return getValidPawnMoves(candidate_moves, moving_piece);
	}

	for (const auto& candidate_move : candidate_moves) {
		if (!isInBounds(candidate_move) ||
			hasFriendlyPiece(candidate_move, moving_piece_color) ||
			hasKingAt(candidate_move)) {
			continue;
		}

		if (moving_piece->canBeObstructed() && isPathObstructed(moving_piece_position, candidate_move)) {
			continue;
		}

		// Create a temporary board copy to test if this move would expose the king
		if (willExposeKing(moving_piece_position, candidate_move, moving_piece_color)) {
			continue;
		}

		valid_moves.emplace_back(candidate_move);
	}

	// handle castling
	if (moving_piece->getType() == PieceType::King) {
		if (canCastle(moving_piece_color, true)) {
			valid_moves.emplace_back(moving_piece_position.row, moving_piece_position.col + 2, PositionType::KingSideCastle);
		}
		if (canCastle(moving_piece_color, false)) {
			valid_moves.emplace_back(moving_piece_position.row, moving_piece_position.col - 2, PositionType::QueenSideCastle);
		}
	}

	return valid_moves;
}

std::vector<Position> Board::getAllValidMoves(const ChessColor& color) {
	std::vector<Position> valid_moves;
	
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			Position current_position(row, col);

			if (!hasPieceAt(current_position)) {
				continue;
			}

			auto& current_piece = getPieceAt(current_position);
			
			if (current_piece->getColor() != color) {
				continue;
			}

			auto current_valid_moves = getValidMoves(current_piece->getPossibleMoves(), current_piece.get());
			MERGE_VECTORS(valid_moves, current_valid_moves);
		}
	}

	return valid_moves;
}

std::vector<Position> Board::getValidPawnMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece) {
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
	if (isValidPawnStep(moving_piece_position, single_step, moving_piece_color)) {
		valid_pawn_moves.emplace_back(single_step);
	}

	// Double step forward
	if (!moving_piece->hasMoved() && isValidPawnStep(moving_piece_position, double_step, moving_piece_color)) {
		valid_pawn_moves.emplace_back(double_step);
	}

	// Regular captures
	if (!hasKingAt(right_capture) && isValidPawnCapture(moving_piece_position, right_capture, moving_piece_color)) {
		valid_pawn_moves.emplace_back(right_capture);
	}

	if (!hasKingAt(left_capture) && isValidPawnCapture(moving_piece_position, left_capture, moving_piece_color)) {
		valid_pawn_moves.emplace_back(left_capture);
	}

	// En passant
	Position en_passant_move = getEnPassantMove(moving_piece);
	if (isInBounds(en_passant_move)) {
		valid_pawn_moves.emplace_back(en_passant_move);
	}

	return valid_pawn_moves;
}

const std::vector<std::unique_ptr<Piece>>& Board::getCapturedPieces(const ChessColor& color) const {
	return (color == ChessColor::White ? white_captured : black_captured);
}

bool Board::isValidMove(const Position& position) {
	return std::find(valid_moves.begin(), valid_moves.end(), position) != valid_moves.end();
}

bool Board::isCastlingMove(const Position& position) {
	auto find_position = std::find(valid_moves.begin(), valid_moves.end(), position);
	
	if (find_position == valid_moves.end()) {
		return false;
	}

	if (find_position->type == PositionType::KingSideCastle || find_position->type == PositionType::QueenSideCastle) {
		return true;
	}

	return false;
}

Piece* Board::getSelectedPiece() {
	return selected_piece;
}

void Board::selectPiece(const Position& position, const ChessColor& current_turn) {
	valid_moves.clear();

	selected_piece = getPieceAt(position).get();

	if (!selected_piece) {
		selected_piece = nullptr;
		return;
	}

	LOG_INFO("A Piece of code ", selected_piece->getPieceCode(), " was selected");
	
	if (current_turn == selected_piece->getColor()) {
		valid_moves = getValidMoves(selected_piece->getPossibleMoves(), selected_piece);
	}
}

void Board::unselectPiece() {
	selected_piece = nullptr;
	valid_moves.clear();
}

bool Board::isWhiteSide() const {
	return geometry.isWhiteSide();
}

void Board::flip() {
	geometry.flip();
}

std::string Board::computeHash(int turn_count) {
	std::string board_hash;
	std::string placement_field = computePlacementField();
	char active_color = getActiveColor();
	std::string castling_rights = getCastlingRights();
	std::string en_passant_target = getEnPassantTarget();
	std::string halfmove_clock = std::to_string(this->halfmove_clock);
	std::string fullmove_clock = std::to_string(turn_count / 2 + 1);

	board_hash = std::format("{} {} {} {} {} {}", 
		placement_field, active_color,
		castling_rights, en_passant_target,
		halfmove_clock, fullmove_clock);

	LOG_INFO('\n', "Board Hash: ", board_hash);
	return board_hash;
}

void Board::loadFromHash(const std::string& board_hash) {
	std::string placement_field = board_hash.substr(0, board_hash.find(' '));

	LOG_INFO("Decoding: ", placement_field);
	int row = 7, col = 0;
	for (const char& c : placement_field) {
		if (c == '/') {
			row--;
			col = 0;
		}
		else if (isdigit(c)) {
			int empty_tiles = c - '0';

			while (empty_tiles--) {
				setPieceAt(Position(row, col), nullptr);
				col++;
			}

			continue;
		}
		else {
			std::unique_ptr<Piece> new_piece = std::make_unique<Piece>(typeFromSymbol(c), colorFromSymbol(c), Position(row, col));
			setPieceAt(Position(row, col), std::move(new_piece));
			col++;
		}
	}
}

void Board::renderHash(const std::string& board_hash, sf::RenderWindow& window) {
	auto decoded_board = std::make_unique<Board>(board_hash);
	decoded_board->render(window);
}

BoardGeometry& Board::getGeometry() {
	return geometry;
}

bool Board::isLastMovePromotion() {
	if (!last_move) {
		return false;
	}

	auto& moving_piece = getPieceAt(last_move->to);

	if (!moving_piece) {
		return false;
	}

	if (moving_piece->getType() != PieceType::Pawn) {
		return false;
	}

	return moving_piece->getColor() == ChessColor::White ? moving_piece->getPosition().row == 7 : moving_piece->getPosition().row == 0;
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

void Board::copy(const Board& other) {
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
	halfmove_clock = other.halfmove_clock;

	tile_states = other.tile_states;
}

void Board::clear() {
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			board[row][col].reset();
		}
	}

	white_captured.clear();
	black_captured.clear();
	delete last_move;
	last_move = nullptr;
	selected_piece = nullptr;
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
	float overlay_scale = 1;
	if (!overlay_name.empty()) {
		overlay_scale = geometry.getTileSize() / AssetManager::getInstance().getTexture(overlay_name).getSize().x;
	}
	
	sf::Vector2f tile_position = geometry.getTilePosition(position);
	
	tile_sprite.setScale(tile_scale, tile_scale);
	overlay_sprite.setScale(overlay_scale, overlay_scale);

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

	if (isKingChecked(ChessColor::White, true)) {
		auto position = getKingPosition(ChessColor::White);
		tile_states[position.row][position.col] = TileState::Attacked;
	}

	if (isKingChecked(ChessColor::Black, true)) {
		auto position = getKingPosition(ChessColor::Black);
		tile_states[position.row][position.col] = TileState::Attacked;
	}

	if (selected_piece) {
		auto selected_position = selected_piece->getPosition();
		tile_states[selected_position.row][selected_position.col] = TileState::Selected;
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
	auto find_position = std::find(valid_moves.begin(), valid_moves.end(), position);

	if (find_position != valid_moves.end()) {
		auto& current_piece = getPieceAt(position);

		if (current_piece) {
			return TileState::Attacked;
		}

		if (find_position->type == PositionType::EnPassant || find_position->type == PositionType::KingSideCastle
			|| find_position->type == PositionType::QueenSideCastle) {
			return TileState::Special;
		}

		return TileState::Highlighted;
	}

	if ((last_move) && (last_move->from == position || last_move->to == position)) {
		return TileState::LastMove;
	} // handle last move

	return TileState::None;
}

bool Board::isInBounds(const Position& position) const {
	return position.row < 8 && position.row >= 0 && position.col < 8 && position.col >= 0;
}

bool Board::hasFriendlyPiece(const Position& position, const ChessColor& color) {
	return hasPieceAt(position) && color == getPieceAt(position)->getColor();
}


bool Board::hasKingAt(const Position& position) {
	return isInBounds(position) && hasPieceAt(position) && getPieceAt(position)->getType() == PieceType::King;
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

bool Board::willExposeKing(const Position& from, const Position& to, const ChessColor& king_color) {
	// Create a temporary board copy to test if this move would expose the king
	Board temp_board(*this);
	Move temp_move(from, to, nullptr);
	temp_board.makeMove(temp_move);
	return temp_board.isKingChecked(king_color, true);
}

bool Board::isValidPawnStep(const Position& from, const Position& to, const ChessColor& king_color) {
	return isInBounds(to) &&
		!hasPieceAt(to) && 
	    !isPathObstructed(from, to) && 
		!willExposeKing(from, to, king_color);
}

bool Board::isValidPawnCapture(const Position& from, const Position& to, const ChessColor& king_color) {
	ChessColor opponent_color = (king_color == ChessColor::White ? ChessColor::Black : ChessColor::White);
	
	if (abs(from.row - to.row) != 1 || abs(from.col - to.col) != 1) {
		return false;
	}

	return isInBounds(to) && !willExposeKing(from, to, king_color) && hasPieceAt(to) && getPieceAt(to)->getColor() == opponent_color;
}


Position Board::getEnPassantMove(Piece* moving_piece) {
	Position invalid_move = { -1, -1 };

	// moving piece data
	ChessColor moving_piece_color = moving_piece->getColor();
	ChessColor opponent_color = (moving_piece_color == ChessColor::White ? ChessColor::Black : ChessColor::White);
	Position moving_piece_position = moving_piece->getPosition();

	// last moved piece data
	if (!last_move) {
		return invalid_move;
	}

	auto& last_moved_piece = getPieceAt(last_move->to);

	if (!last_moved_piece) {
		return invalid_move;
	}

	ChessColor last_moved_piece_color = last_moved_piece->getColor();
	PieceType last_moved_piece_type = last_moved_piece->getType();
	Position last_moved_piece_position = last_moved_piece->getPosition();

	// The en passant capture must be performed on the turn immediately after the pawn being captured moves.
	if (last_moved_piece_type != PieceType::Pawn || last_moved_piece_color != opponent_color) {
		return invalid_move;
	}
	
	// The captured pawn must have moved two squares in one move, landing right next to the capturing pawn.
	int vertical_distance = abs(last_move->from.row - last_move->to.row);
	if (vertical_distance != 2) {
		return invalid_move;
	} 

	// The en passante can only be done on this row
	int en_passant_row = (moving_piece_color == ChessColor::White ? 4 : 3);
	if (last_moved_piece_position.row != en_passant_row || last_moved_piece_position.row != moving_piece_position.row) {
		return invalid_move;
	} 

	// check if the captured pawn is directly next to the en passant pawn
	int col_distance = abs(last_moved_piece_position.col - moving_piece_position.col);
	if (col_distance != 1) {
		return invalid_move;
	} 

	// Calculate en passant move
	int direction = (moving_piece_color == ChessColor::White ? 1 : -1);
	Position en_passant_move(moving_piece_position.row + direction, last_moved_piece_position.col, PositionType::EnPassant);
	if (isInBounds(en_passant_move) && !willExposeKing(moving_piece_position, en_passant_move, moving_piece_color) && !hasKingAt(en_passant_move)) {
		return en_passant_move;
	}

	return invalid_move;
}

void Board::makeCastlingMove(const Move& king_move) {
	// get rook move data
	bool is_king_side = king_move.getType() == PositionType::KingSideCastle;
	Position rook_from(king_move.from.row, (is_king_side ? 7 : 0));
	Position rook_to(king_move.from.row, (is_king_side ? 5 : 3));
	Move rook_move(rook_from, rook_to);

	// apply rook move
	makeMove(rook_move);
}

bool Board::canCastle(const ChessColor& color, bool king_side) {
	// get king
	auto king_position = getKingPosition(color);
	auto& king = getPieceAt(king_position);

	// king not moved
	if (king->hasMoved() || king_position.row != (color == ChessColor::White ? 0 : 7)) {
		return false;
	}

	// get rook
	auto rook_position = Position(king_position.row, (king_side ? 7 : 0));
	auto& rook = getPieceAt(rook_position);

	// rook moved or not found in place
	if (!rook || rook->getType() != PieceType::Rook || rook->getColor() != color || rook->hasMoved()) {
		return false;
	}

	// path between king and rook is obstructed
	if (isPathObstructed(king_position, rook_position)) {
		return false;
	}

	// path between king and rook is not safe
	int rook_direction = (king_side ? 1 : -1);
	if (willExposeKing(king_position, Position(king_position.row, king_position.col + rook_direction), color)
		|| willExposeKing(king_position, Position(king_position.row, king_position.col + rook_direction * 2), color)) {
		return false;
	}

	return true;
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

std::string Board::computePlacementField() {
	std::string placement_field;

	for (int row = 0; row < 8; ++row) {
		placement_field += computeRowPlacement(row);
	}

	return placement_field;
}

std::string Board::computeRowPlacement(int row) {
	std::string placement_field;

	int empty_tile_count = 0;
	for (int col = 0; col < 8; ++col) {
		auto& current_piece = getPieceAt(Position(row, col));
		if (current_piece) {

			if (empty_tile_count > 0) {
				placement_field += std::to_string(empty_tile_count);
				empty_tile_count = 0;
			}

			placement_field.push_back(getPieceSymbol(current_piece->getType(), current_piece->getColor()));
		}
		else {
			empty_tile_count++;
		}

	}

	if (empty_tile_count > 0) {
		placement_field += std::to_string(empty_tile_count);
	}

	if (row != 7) {
		placement_field.push_back('/');
	}

	return placement_field;
}

char Board::getPieceSymbol(const PieceType& type, const ChessColor& color) {
	char piece_symbol = '*';

	switch (type) {
		case PieceType::Pawn:
			piece_symbol = 'p';
			break;

		case PieceType::Rook:
			piece_symbol = 'r';
			break;

		case PieceType::Knight:
			piece_symbol = 'n';
			break;

		case PieceType::Bishop:
			piece_symbol = 'b';
			break;

		case PieceType::Queen:
			piece_symbol = 'q';
			break;

		case PieceType::King:
			piece_symbol = 'k';
			break;

		default:
			break;
	}

	if (color == ChessColor::White) {
		piece_symbol = std::toupper(piece_symbol);
	}

	return piece_symbol;
}

PieceType Board::typeFromSymbol(const char& symbol) {
	PieceType piece_type;

	switch (std::tolower(symbol)) {
		case 'p': piece_type = PieceType::Pawn; break;
		case 'r': piece_type = PieceType::Rook; break;
		case 'n': piece_type = PieceType::Knight; break;
		case 'b': piece_type = PieceType::Bishop; break;
		case 'q': piece_type = PieceType::Queen; break;
		case 'k': piece_type = PieceType::King; break;
		default: break;
	}

	return piece_type;
}

ChessColor Board::colorFromSymbol(const char& symbol) {
	return (isupper(symbol) ? ChessColor::White : ChessColor::Black);
}

char Board::getActiveColor() {
	char active_color = 'w';
	if (last_move) {
		active_color = (getPieceAt(last_move->to)->getColor() == ChessColor::White ? 'b' : 'w');
	}

	return active_color;
}

std::string Board::getCastlingRights() {
	std::string castling_rights;

	if (canCastle(ChessColor::White, true)) {
		castling_rights.push_back('K');
	}

	if (canCastle(ChessColor::White, false)) {
		castling_rights.push_back('Q');
	}

	if (canCastle(ChessColor::Black, true)) {
		castling_rights.push_back('k');
	}

	if (canCastle(ChessColor::Black, false)) {
		castling_rights.push_back('q');
	}


	return (castling_rights.size() == 0 ? "-" : castling_rights);
}

std::string Board::getEnPassantTarget() {
	std::string en_passant_target = "-";

	if (!last_move) {
		return en_passant_target;
	}

	auto& last_moved_piece = getPieceAt(last_move->to);
	if (last_moved_piece && last_moved_piece->getType() == PieceType::Pawn) {
		int vertical_distance = abs(last_move->from.row - last_move->to.row);

		if (vertical_distance == 2) {
			en_passant_target = getAlgebraicNotation(last_move->to);
		}

	}

	return en_passant_target;
}

std::string Board::getAlgebraicNotation(const Position& position) {
	if (!isInBounds(position)) {
		return "";
	}

	char file = char('a' + position.col);
	char rank = char('0' + position.row + 1);
	std::string algebraic_notation;
	algebraic_notation.push_back(file);
	algebraic_notation.push_back(rank);

	return algebraic_notation;
}
