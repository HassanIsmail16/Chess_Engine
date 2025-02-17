#include "MoveHistoryGeometry.h"
#include "../managers/AssetManager.h"

void MoveHistoryGeometry::update(const sf::RenderWindow& window, const float& scroll_percent, const int& entry_count) {
    float window_width = static_cast<float>(window.getSize().x);
    float window_height = static_cast<float>(window.getSize().y);
    
    margin = window_width * margin_percent;
    sf::Sprite panel = AssetManager::getInstance().getSprite("move-history");
    float panel_width = panel.getTexture()->getSize().x;
	float panel_height = panel.getTexture()->getSize().y;

    body_scale = panel.getScale();
    body_width = window_width * 0.261f;
    body_height = (body_width * panel_height) / panel_width;
	body_scale = sf::Vector2f(body_width / panel_width, body_height / panel_height);
    body_x = window_width - (margin + body_width);
    body_y = margin + window_height * 0.19167f;
    
    entry_start_x = body_x + (0.21257f * body_width);
    entry_start_y = body_y + (0.1185f * body_height);
    entry_width = body_width * 0.27f;
    entry_height = body_height * 0.0915f;
    entry_spacing = entry_height * 0.133f;

    this->entry_count = std::max(0, entry_count);
    float row_count = entry_count / 2 + (entry_count % 2);

    total_height = row_count > 0 ? 
        row_count * entry_height + (row_count - 1) * entry_spacing :
        0.0f;

    visible_height = getEntryAreaHeight();
    this->scroll_percent = std::clamp(scroll_percent, 0.0f, 1.0f);
    max_scroll_percent = std::max(0.0f, (total_height - visible_height) / total_height);

    housing_height = 0.6274f * body_height;
    min_knob_height = 0.1146f * housing_height;
    max_knob_height = 0.9483f * housing_height;
    knob_height = std::clamp(max_knob_height * (visible_height / total_height), min_knob_height, max_knob_height);
    knob_width = 0.0695f * body_width;
    knob_x = body_x + 0.817f * body_width;
    knob_start_y = body_y + 0.1764f * body_height;
    float knob_travel = 0.9375f * housing_height - knob_height;
    knob_y = (scroll_percent != 0 ? (knob_start_y) + knob_travel * (scroll_percent / max_scroll_percent) : knob_start_y);

    auto upper_knob_texture = AssetManager::getInstance().getTexture("knob-up");
    auto middle_knob_texture = AssetManager::getInstance().getTexture("knob-center");
    auto lower_knob_texture = AssetManager::getInstance().getTexture("knob-down");

    upper_knob_part_height = (knob_width * upper_knob_texture.getSize().y) / upper_knob_texture.getSize().x;
    lower_knob_part_height = (knob_width * lower_knob_texture.getSize().y) / lower_knob_texture.getSize().x;
    middle_knob_part_height = std::max(0.f, knob_height - (upper_knob_part_height + lower_knob_part_height));

}

sf::Vector2f MoveHistoryGeometry::getEntryPosition(const int& index) const {
    if (index < 0 || index >= entry_count) {
        return sf::Vector2f(-1.0f, -1.0f);
    }
    
    float entry_x = (index % 2 == 0) ? entry_start_x : entry_start_x + entry_width;
    float scrolled_offset = total_height * scroll_percent;
    int row_index = index / 2;
    float entry_y = entry_start_y + (row_index * entry_height) + (std::max(row_index, 0) * entry_spacing) - scrolled_offset;
    
    return sf::Vector2f(entry_x, entry_y);
}

int MoveHistoryGeometry::getEntryIndex(const sf::Vector2i& position) const {
    if (!isInsideEntryRegion(position)) {
        return -1;
    }

    float scrolled_offset = total_height * scroll_percent;
    float relative_y = position.y - entry_start_y + scrolled_offset;
    int row = static_cast<int>(relative_y / (entry_height + entry_spacing));

    if (row < 0 || row >= entry_count) {
        return -1;
    }

    bool is_right_column = position.x >= (entry_start_x + entry_width);
    if (is_right_column && (row * 2 + 1) < entry_count) {
        return row * 2 + 1;
    }
    return row * 2;
}

bool MoveHistoryGeometry::isEntryVisible(const int& index) const {
    if (index < 0 || index >= entry_count) {
        return false;
    }

    float scrolled_offset = total_height * scroll_percent;
    int row_index = index / 2;
    int entry_y = entry_start_y + (row_index * entry_height) + (std::max(row_index, 0) * entry_spacing) - scrolled_offset;
    int entry_x = entry_start_x + (index % 2 == 0 ? 2 : 1) * entry_width;

    return isInsideEntryRegion({entry_x, entry_y});
}

float MoveHistoryGeometry::getEntryWidth() const {
	return entry_width;
}

float MoveHistoryGeometry::getEntryHeight() const {
	return entry_height;
}

float MoveHistoryGeometry::getEntrySpacing() const {
    return entry_spacing;
}

sf::Vector2f MoveHistoryGeometry::getBodyScale() const {
	return body_scale;
}

sf::Vector2f MoveHistoryGeometry::getEntryScale() const {
    auto sprite = AssetManager::getInstance().getSprite("blue-button");
    return {
        entry_width / sprite.getTexture()->getSize().x,
        entry_height / sprite.getTexture()->getSize().y
    };
}

float MoveHistoryGeometry::getBodyWidth() const {
    return body_width;
}

float MoveHistoryGeometry::getBodyHeight() const {
    return body_height;
}

float MoveHistoryGeometry::getEntryAreaWidth() const {
    return 0.5928f * body_width;
}

float MoveHistoryGeometry::getEntryAreaHeight() const {
    return 0.67f * body_height;
}

float MoveHistoryGeometry::getEntryAreaX() const {
    return body_x + (0.1856f * body_width);
}

float MoveHistoryGeometry::getEntryAreaY() const {
    return body_y + (0.104f * body_height);
}

float MoveHistoryGeometry::getBodyX() const {
	return body_x;
}

float MoveHistoryGeometry::getBodyY() const {
	return body_y;
}

float MoveHistoryGeometry::getMargin() const {
	return margin;
}

float MoveHistoryGeometry::getEntryX() const {
    return entry_start_x;
}

float MoveHistoryGeometry::getTotalHeight() const {
    return total_height;
}

float MoveHistoryGeometry::getVisibleHeight() const {
    return visible_height;
}

float MoveHistoryGeometry::getScrollOffset() const {
    return total_height * scroll_percent;
}

float MoveHistoryGeometry::getMaxScrollPercent() const {
    return max_scroll_percent;
}

float MoveHistoryGeometry::getMinKnobHeight() const {
    return min_knob_height;
}

float MoveHistoryGeometry::getMaxKnobHeight() const {
    return max_knob_height;
}

float MoveHistoryGeometry::getFullKnobHeight() const {
    return knob_height;
}

float MoveHistoryGeometry::getUpperKnobHeight() const {
    return upper_knob_part_height;
}

float MoveHistoryGeometry::getMiddleKnobHeight() const {
    return middle_knob_part_height;
}

float MoveHistoryGeometry::getLowerKnobHeight() const {
    return lower_knob_part_height;
}

float MoveHistoryGeometry::getKnobStartY() const {
    return knob_start_y;
}

float MoveHistoryGeometry::getKnobY() const {
    return knob_y;
}

float MoveHistoryGeometry::getKnobX() const {
    return knob_x;
}

float MoveHistoryGeometry::getScrollHousingHeight() const {
    return housing_height;
}

float MoveHistoryGeometry::getKnobWidth() const {
    return knob_width;
}

bool MoveHistoryGeometry::isInsideBody(const sf::Vector2i& position) const {
    return position.x >= body_x &&
        position.x <= body_x + body_width &&
        position.y >= body_y &&
        position.y <= body_y + body_height;
}

bool MoveHistoryGeometry::isInsideBody(const sf::Vector2f& position) const {
    return position.x >= body_x &&
        position.x <= body_x + body_width &&
        position.y >= body_y &&
        position.y <= body_y + body_height;
}

bool MoveHistoryGeometry::isInsideEntryRegion(const sf::Vector2i& position) const {
    return position.x >= entry_start_x &&
        position.x <= entry_start_x + (entry_width * 2) &&
        position.y >= entry_start_y &&
        position.y <= entry_start_y + visible_height;
}

void MoveHistoryGeometry::setMarginPercent(const float& percent) {
	margin_percent = percent;
}