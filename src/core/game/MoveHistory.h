#pragma once
#include "BoardUtilities.h"
#include "Board.h"
#include "../managers/InputManager.h"
#include "MoveHistoryGeometry.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct MoveEntry {
	MoveEntry(const Move& move, const std::string& board_hash, const std::string& algebraic_notation) : move(move), board_hash(board_hash), algebraic_notation(algebraic_notation) {}
	MoveEntry(const Move& move, const Move& rook_move, const std::string& board_hash, const std::string& algebraic_notation): move(move), rook_move(rook_move), board_hash(board_hash), algebraic_notation(algebraic_notation) {}
	bool isCastling() { return move.to.type == PositionType::KingSideCastle || move.to.type == PositionType::QueenSideCastle; }
	Move move;
	Move rook_move;
	std::string board_hash;
	std::string algebraic_notation;
};

class MoveHistory {
public:
	MoveHistory();
	void recordMove(const Move& move, const std::string& hash, const std::string& algebraic_notation);
	void recordMove(const MoveEntry& entry);
	bool canUndo();
	bool canRedo();
	bool canJumpTo(const int& index);
	void undo();
	void redo();
	void jumpToMove(const int& index);
	void jumpToPreviousMove();
	void jumpToNextMove();
	void reset();
	int getCurrentMoveIndex();
	int getTotalMoves();
	bool isEmpty() const;
	const std::vector<MoveEntry>& getMoveHistory() const;
	const Move& getLastMove() const;

	MoveHistoryGeometry getGeometry() const;

	void update(const float& dt);
	void render(sf::RenderWindow& window);
	void handleInput(const InputManager& input_manager);
private:
	void renderEntry(MoveEntry entry, const sf::Vector2f& position, int index);
	void renderScrollBar(sf::RenderWindow& window);
	void renderScrollBarArrows(sf::RenderWindow& window);
	void renderScrollBarKnob(sf::RenderWindow& window);

	void scroll(const float& scroll_amount);

	void initializeView(sf::RenderWindow& window);
	
	void recordCastlingMove(const Move& move, const std::string& hash, const std::string& algebraic_notation);
	int current_index;
	std::vector<MoveEntry> moves;

	MoveHistoryGeometry geometry;
	bool scroll_sync_pending = false;
	float scroll_percent = 0.0f;
	float scroll_velocity;
	float scroll_damping_factor = 3.0f;
	float dt = 1.0f;
	bool is_dragging = false;
	float drag_start_y = 0.0f;

	std::unique_ptr<sf::View> view;
	std::unique_ptr<sf::RenderTexture> mask;
	bool geometry_initialized = false;
};

