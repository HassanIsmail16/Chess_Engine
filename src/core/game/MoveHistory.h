#pragma once
#include "BoardUtilities.h"
#include "Board.h"
#include "../managers/InputManager.h"
#include "MoveHistoryGeometry.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct MoveEntry {
	MoveEntry(const Move& move, const std::string& board_hash) : move(move), board_hash(board_hash) {}
	MoveEntry(const Move& move, const Move& rook_move, const std::string& board_hash): move(move), rook_move(rook_move), board_hash(board_hash) {}
	bool isCastling() { return move.to.type == PositionType::KingSideCastle || move.to.type == PositionType::QueenSideCastle; }
	Move move;
	Move rook_move;
	std::string board_hash;
};

class MoveHistory {
public:
	MoveHistory();
	void recordMove(const Move& move, const std::string& hash);
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

	void render(sf::RenderWindow& window);
	void handleInput(const InputManager& input_manager);
private:
	void renderEntry(sf::RenderWindow& window, MoveEntry entry, const sf::Vector2f& position);

	void recordCastlingMove(const Move& move, const std::string& hash);
	std::string formatMove(Move move) const;
	
	int current_index;
	std::vector<MoveEntry> moves;

	MoveHistoryGeometry geometry;

	float scroll_percent = 0.0f;
	
};

