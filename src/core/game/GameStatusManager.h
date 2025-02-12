#pragma once
#include "Board.h"

class GameStatusManager {
public:
	GameStatusManager();
	bool isCheckMate(Board& board, const ChessColor& color);
	bool isDraw(Board& board);
	bool isInsufficientMaterials(Board& board);
	bool isThreeFoldRepetition(Board& board);
	bool isStaleMate(Board& board);
	bool isStaleMate(Board& board, const ChessColor& color);

	void registerBoard(const std::string& board_hash);
	void unregisterBoard(const std::string& board_hash);

	void nextTurn();
	void previousTurn();
	ChessColor getCurrentTurn() const;
	void endTurn(Board& board);
	int getTurnCount() const;

private:
	std::string cleanUpHash(const std::string& board_hash);

	ChessColor current_turn;
	int turn_count;
	std::unordered_map<std::string, int> repetition_tracker;
};