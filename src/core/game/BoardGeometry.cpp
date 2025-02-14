#include "BoardGeometry.h"
#include "../managers/AssetManager.h"

void BoardGeometry::update(const sf::RenderWindow& window) {
	margin = window.getSize().x * margin_percent;

	board_size = window.getSize().y - (margin * 2);
	
	sf::Sprite board_frame = AssetManager::getInstance().getSprite("board-frame");
	board_frame_size = board_frame.getTexture()->getSize().x;
	board_scale = board_size / board_frame_size;
	
	board_x = window.getSize().x - (board_frame_size * board_scale) - margin;

	frame_margin = board_scale * board_frame_size * 0.10f;
	
	tile_size = (board_size - frame_margin * 2) / 8;

	tile_start_x = board_x + frame_margin;
	tile_start_y = margin + frame_margin;
}

sf::Vector2f BoardGeometry::getTilePosition(const Position& position) const {
	if (is_white_side) {
		return {
			tile_size * position.col + tile_start_x,
			tile_size * (7 - position.row) + tile_start_y
		};
	} 

	return {
		tile_size * position.col + tile_start_x,
		tile_size * position.row + tile_start_y
	};
}

Position BoardGeometry::getTileBoardPosition(const sf::Vector2i& position) {
	sf::Vector2f position_f(position.x, position.y);

	float tile_x = position_f.x - tile_start_x;
	float tile_y = position_f.y - tile_start_y;

	if (is_white_side) {
		return {
			7 - (int) tile_y / (int) tile_size,
			(int) tile_x / (int) tile_size
		};
	}

	return {
		(int) tile_y / (int) tile_size,
		(int) tile_x / (int) tile_size
	};
}

float BoardGeometry::getTileSize() const {
	return tile_size;
}

float BoardGeometry::getBoardScale() const {
	return board_scale;
}

float BoardGeometry::getBoardX() const {
	return board_x;
}

float BoardGeometry::getMargin() const {
	return margin;
}

bool BoardGeometry::isInsideBoardTiles(const sf::Vector2i& position) const {
	float tile_end_x = tile_start_x + 8 * tile_size;
	float tile_end_y = tile_start_y + 8 * tile_size;
	return position.x >= tile_start_x && position.x <= tile_end_x
		&& position.y >= tile_start_y && position.y <= tile_end_y;
}

void BoardGeometry::setMarginPercent(const float& percent) {
	margin_percent = percent;
}

void BoardGeometry::flip() {
	is_white_side = !is_white_side;
}

bool BoardGeometry::isWhiteSide() const {
	return is_white_side;
}
