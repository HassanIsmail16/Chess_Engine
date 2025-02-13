#include "MoveHistoryGeometry.h"
#include "../managers/AssetManager.h"

void MoveHistoryGeometry::update(const sf::RenderWindow& window, const float& scroll_percent, const int& entry_count) {
    float window_width = static_cast<float>(window.getSize().x);
    float window_height = static_cast<float>(window.getSize().y);
    
    margin = window_width * margin_percent;
    sf::Sprite panel = AssetManager::getInstance().getSprite("move-history-panel");
    body_scale = panel.getScale();
    // TODO: make it dynamically scaled
    body_width = panel.getTexture()->getSize().x;
    body_height = (body_width * body_scale.y) / body_scale.x;
    body_x = window_width - (margin + body_width);
    body_y = margin + window_height * 0.19167f;
    
    entry_start_x = body_x + window_width * 0.046875f;
    entry_start_y = body_y + window_height * 0.04167f;
    entry_spacing = window_height * 0.0138f;
    entry_width = window_width * 0.09375f;
    entry_height = window_height * 0.051388f;
    visible_height = window_height * 0.2638f;
    
    this->scroll_percent = std::clamp(scroll_percent, 0.0f, 1.0f);
    this->entry_count = std::max(0, entry_count);
    
    total_height = entry_count > 0 ? 
        entry_count * entry_height + (entry_count - 1) * entry_spacing : 
        0.0f;
}

sf::Vector2f MoveHistoryGeometry::getEntryPosition(const int& index) const {
    if (!isEntryVisible(index) || index < 0 || index >= entry_count) {
        return sf::Vector2f(-1.0f, -1.0f);
    }
    
    float entry_x = (index % 2 == 0) ? entry_start_x : entry_start_x + entry_width;
    float scrolled_offset = total_height * scroll_percent;
    float entry_y = entry_start_y + (index * (entry_height + entry_spacing)) - scrolled_offset;
    
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
    float entry_y = entry_start_y + (index * (entry_height + entry_spacing)) - scrolled_offset;
    
    return entry_y >= entry_start_y - entry_height && 
           entry_y <= entry_start_y + visible_height;
}

float MoveHistoryGeometry::getEntryWidth() const {
	return entry_width;
}

float MoveHistoryGeometry::getEntryHeight() const {
	return entry_height;
}

sf::Vector2f MoveHistoryGeometry::getBodyScale() const {
	return body_scale;
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

bool MoveHistoryGeometry::isInsideBody(const sf::Vector2i& position) const {
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