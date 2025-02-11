#pragma once
#include <SFML/Graphics.hpp>
#include "BoardUtilities.h"

class BoardGeometry {
public:
	void update(const sf::RenderWindow& window);

	sf::Vector2f getTilePosition(const Position& position) const;
	Position getTileBoardPosition(const sf::Vector2i& position);

	float getTileSize() const;
	float getBoardScale() const;
	float getBoardX() const;
	float getMargin() const;

	void setMarginPercent(const float& percent);

private:
	float margin_percent = 0.05;
	float margin;
	float board_size;
	float board_frame_size;
	float board_scale;
	float board_x;
	float frame_margin;
	float tile_size;
	float tile_start_x;
	float tile_start_y;
};