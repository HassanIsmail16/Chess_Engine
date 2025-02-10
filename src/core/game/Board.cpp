#include "Board.h"
#include "../log/Logger.hpp"
#include "Piece.h"
#include "../managers/AssetManager.h"

Board::Board() {
	selected_piece = nullptr;
	last_move = nullptr;
	this->initializeBoard();
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

uint64_t Board::computeHash() const {
	return 0;
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

std::vector<Position> Board::getValidMoves(std::vector<Position>& candidate_moves, Piece* moving_piece) {
	return std::vector<Position>();
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
