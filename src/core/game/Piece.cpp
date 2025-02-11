#include "Piece.h"
#include "../log/Logger.hpp"

Piece::Piece(PieceType type, ChessColor color, Position position) : 
	type(type), 
	color(color), 
	current_position(position), 
	move_count(0) {
	sprite = AssetManager::getInstance().getSprite(getPieceCode());
}

Piece::Piece(const Piece& other) : 
	type(other.type), 
	color(other.color), 
	current_position(other.current_position), 
	move_count(other.move_count), 
	sprite(other.sprite)
{}

void Piece::update(const float& dt) {
	return;
}

void Piece::render(sf::RenderWindow& window, const BoardGeometry& geometry) {
	auto tile_position = geometry.getTilePosition(current_position);
	sprite.setPosition(tile_position.x + 3, tile_position.y + 3);
	window.draw(sprite);
}

void Piece::setPosition(const Position new_position) {
	current_position = new_position;
}


std::unique_ptr<Piece> Piece::clone() const {
	return std::make_unique<Piece>(*this);
}

Position Piece::getPosition() {
	return current_position;
}

void Piece::incrementMoveCount() {
	move_count++;
}

void Piece::decrementMoveCount() {
	if (move_count <= 0) {
		LOG_WARNING("Attempted to decrement the move_count of piece ", getPieceCode(),
			" at row ", current_position.row, ", col ", current_position.col, " while it has the value ", move_count);
	}

	move_count--;
}

std::vector<Position> Piece::getPossibleMoves() const {
	switch (type) {
		case PieceType::Pawn:
		return getPawnPossibleMoves();
		break;

		case PieceType::Rook:
		return getRookPossibleMoves();
		break;

		case PieceType::Bishop:
		return getBishopPossibleMoves();
		break;

		case PieceType::Knight:
		return getKnightPossibleMoves();
		break;

		case PieceType::Queen:
		return getQueenPossibleMoves();
		break;

		case PieceType::King:
		return getKingPossibleMoves();
		break;

		default:
		LOG_WARNING("Invoked Piece::getPossibleMoves() on a piece of unknown type, "
			"row: ", current_position.row, ", col: ", current_position.col);
		break;
	}
}

bool Piece::hasMoved() const {
	return move_count;
}

bool Piece::canBeObstructed() const {
	return (type == PieceType::Rook || type == PieceType::Bishop || type == PieceType::Queen);
}

PieceType Piece::getType() const {
	return type;
}

ChessColor Piece::getColor() const {
	return color;
}

Position Piece::getPosition() const {
	return current_position;
}
std::string Piece::getPieceCode() {
	std::string type_code;
	
	switch (type) {
		case PieceType::Pawn: 
		type_code = "pawn";
		break;

		case PieceType::Rook: 
		type_code = "rook";
		break;

		case PieceType::Knight: 
		type_code = "knight";
		break;

		case PieceType::Bishop: 
		type_code = "bishop";
		break;

		case PieceType::Queen: 
		type_code = "queen";
		break;

		case PieceType::King: 
		type_code = "king";
		break;

		default: 
		type_code = "unknown";
		break;
	}

	std::string color_code = (color == ChessColor::White ? "w" : "b");

	return type_code + "-" + color_code;
}

std::vector<Position> Piece::getPawnPossibleMoves() const {
	int direction = (color == ChessColor::White ? 1 : -1);
	int row = current_position.row, col = current_position.col;

	return {
		{row + direction, col}, // single step
		{row + 2 * direction, col}, // double step
		{row, col + 1}, // right capture
		{row, col - 1} // left capture
	};
}

std::vector<Position> Piece::getKnightPossibleMoves() const {
	int row = current_position.row, col = current_position.col;
	return {
		{row - 2, col - 1},
		{row - 2, col + 1},
		{row - 1, col + 2},
		{row + 1, col + 2},
		{row + 2, col + 1},
		{row + 2, col - 1},
		{row + 1, col - 2},
		{row - 1, col - 2}
	};
}

std::vector<Position> Piece::getRookPossibleMoves() const {
	auto north = getLinePossibleMoves(-1, 0),
		 south = getLinePossibleMoves(1, 0),
		 east = getLinePossibleMoves(0, 1),
		 west = getLinePossibleMoves(0, -1);

	std::vector<Position> possible_positions;

	MERGE_VECTORS(possible_positions, north);
	MERGE_VECTORS(possible_positions, south);
	MERGE_VECTORS(possible_positions, east);
	MERGE_VECTORS(possible_positions, west);

	return possible_positions;
}

std::vector<Position> Piece::getBishopPossibleMoves() const {
	auto north_east = getLinePossibleMoves(-1, 1),
		 south_east = getLinePossibleMoves(1, 1),
		 south_west = getLinePossibleMoves(1, -1),
		 north_west = getLinePossibleMoves(-1, -1);

	std::vector<Position> possible_positions;

	MERGE_VECTORS(possible_positions, north_east);
	MERGE_VECTORS(possible_positions, south_east);
	MERGE_VECTORS(possible_positions, south_west);
	MERGE_VECTORS(possible_positions, north_west);

	return possible_positions;
}

std::vector<Position> Piece::getQueenPossibleMoves() const {
	auto straights = getRookPossibleMoves(),
		 diagonals = getBishopPossibleMoves();

	std::vector<Position> possible_positions;

	MERGE_VECTORS(possible_positions, straights);
	MERGE_VECTORS(possible_positions, diagonals);

	return possible_positions;
}

std::vector<Position> Piece::getKingPossibleMoves() const {
	int row = current_position.row, col = current_position.col;

	return {
		{row - 1, col}, // up
		{row - 1, col + 1}, // up right
		{row, col + 1}, // right
		{row + 1, col + 1}, // down right
		{row + 1, col}, // down 
		{row + 1, col - 1}, // down left
		{row, col - 1}, // left
		{row - 1, col - 1} // up left
	};
}

std::vector<Position> Piece::getLinePossibleMoves(const int& dx, const int& dy) const {
	std::vector<Position> possible_positions;
	
	for (int row = current_position.row + dy, col = current_position.col + dx; row < 8 && col < 8 && row >= 0 && col >= 0; row += dy, col += dx) {
		possible_positions.emplace_back(row, col);
	}

	return possible_positions;
}
