#pragma once
#include <SFML/Graphics.hpp>

class MoveHistoryGeometry {
public:
	void update(const sf::RenderWindow& window, const float& scroll_percent, const int& entry_count);

	sf::Vector2f getEntryPosition(const int& index) const;
	int getEntryIndex(const sf::Vector2i& position) const;

	bool isEntryVisible(const int& index) const;

	float getEntryWidth() const;
	float getEntryHeight() const;
	sf::Vector2f getBodyScale() const;
	float getBodyX() const;
	float getBodyY() const;
	float getMargin() const;

	bool isInsideBody(const sf::Vector2i& position) const;
	bool isInsideEntryRegion(const sf::Vector2i& position) const;
	void setMarginPercent(const float& percent);

private:
    float margin_percent = 0.05f;
    float margin = 0.0f;
    float body_width = 0.0f;
    float body_height = 0.0f;
    sf::Vector2f body_scale;
    float body_x = 0.0f;
    float body_y = 0.0f;
    float entry_width = 0.0f;
    float entry_height = 0.0f;
    float entry_start_x = 0.0f;
    float entry_start_y = 0.0f;
    float entry_spacing = 0.0f;
    float scroll_percent = 0.0f;
    float visible_height = 0.0f;
    float total_height = 0.0f;
    int entry_count = 0;
};