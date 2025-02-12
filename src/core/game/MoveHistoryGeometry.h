#pragma once
#include <SFML/Graphics.hpp>

class MoveHistoryGeometry {
public:
	void update(const sf::RenderWindow& window);

	sf::Vector2f getEntryPosition(const int& index) const;
	int getEntryIndex(const sf::Vector2f& position) const;

	float getEntryWidth() const;
	float getEntryHeight() const;
	float getBodyScale() const;
	float getBodyX() const;
	float getBodyY() const;
	float getMargin() const;

	bool isInsideBody(const sf::Vector2f& position) const;

	void setMarginPercent(const float& percent);

private:
	float margin_percent = 0.05f;
	float margin;
	float body_width;
	float body_height;
	float body_scale;
	float body_x;
	float body_y;
	float entry_width;
	float entry_height;

};