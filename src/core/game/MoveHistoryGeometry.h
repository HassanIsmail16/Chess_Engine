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
    float getEntrySpacing() const;
	sf::Vector2f getBodyScale() const;
    sf::Vector2f getEntryScale() const;
    float getBodyWidth() const;
    float getBodyHeight() const;
    float getEntryAreaWidth() const;
    float getEntryAreaHeight() const;
    float getEntryAreaX() const;
    float getEntryAreaY() const;
	float getBodyX() const;
	float getBodyY() const;
	float getMargin() const;
    float getEntryX() const;
    float getTotalHeight() const;
    float getVisibleHeight() const;
    float getScrollOffset() const;
    float getMaxScrollPercent() const;
    float getMinKnobHeight() const;
    float getMaxKnobHeight() const;
    float getFullKnobHeight() const;
    float getUpperKnobHeight() const;
    float getMiddleKnobHeight() const;
    float getLowerKnobHeight() const;
    float getKnobStartY() const;
    float getKnobY() const;
    float getKnobX() const;
    float getScrollHousingHeight() const;
    float getKnobWidth() const;
    float getArrowX() const;
    float getUpArrowY() const;
    float getDownArrowY() const;
    float getArrowWidth() const;
    float getArrowHeight() const;
    sf::Vector2f getArrowScale() const;
	bool isInsideBody(const sf::Vector2i& position) const;
    bool isInsideBody(const sf::Vector2f& position) const;
	bool isInsideEntryRegion(const sf::Vector2i& position) const;
    bool isAboveScrollBarKnob(const sf::Vector2i& position) const;
    bool isInsideScrollBarKnob(const sf::Vector2i& position) const;
    bool isBelowScrollBarKnob(const sf::Vector2i& position) const;
    bool isInsideScrollBarHousing(const sf::Vector2i position) const;
    bool isInsideUpperArrow(const sf::Vector2i& position) const;
    bool isInsideLowerArrow(const sf::Vector2i& position) const;
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
    float max_scroll_percent;
    float visible_height = 0.0f;
    float total_height = 0.0f;
    float housing_height;
    float min_knob_height;
    float max_knob_height;
    float knob_height;
    float knob_x;
    float knob_start_y;
    float knob_y;
    float knob_width;
    float upper_knob_part_height;
    float lower_knob_part_height;
    float middle_knob_part_height;
    float arrow_x;
    float up_arrow_y;
    float down_arrow_y;
    float arrow_width;
    float arrow_height;
    sf::Vector2f arrow_scale;
    int entry_count = 0;
};