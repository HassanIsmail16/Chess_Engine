#include "MoveHistory.h"
#include "../log/Logger.hpp"

MoveHistory::MoveHistory() {
	reset();
}

void MoveHistory::recordMove(const Move& move, const std::string& hash) {
	if (move.isCastling()) {
		recordCastlingMove(move, hash);
		return;
	}

	recordMove(MoveEntry(move, hash));
}

void MoveHistory::recordMove(const MoveEntry& entry) {
	if (current_index < (int) moves.size() - 1) {
		LOG_INFO("Truncating move history vector, current_index: ", current_index, ", history size: ", moves.size());
		moves.erase(moves.begin() + current_index + 1, moves.end());
	}

	moves.push_back(entry);
	current_index++;
}

bool MoveHistory::canUndo() {
	return current_index > 0;
}

bool MoveHistory::canRedo() {
	return current_index < moves.size() - 1;
}

bool MoveHistory::canJumpTo(const int& index) {
	return index >= 0 && index < moves.size();
}

void MoveHistory::undo() {
	jumpToPreviousMove();
	moves.pop_back();
}

void MoveHistory::redo() {
	jumpToNextMove();
}

void MoveHistory::jumpToMove(const int& index) {
	if (index < 0 || index >= moves.size() - 1) {
		LOG_ERROR("Tried to jump to move with index ", index, ", number of recorded moves: ", moves.size());
		throw std::runtime_error("Invalid Move History Jump");
	}

	current_index = index;
}

void MoveHistory::jumpToPreviousMove() {
	jumpToMove(current_index - 1);
}

void MoveHistory::jumpToNextMove() {
	jumpToMove(current_index + 1);
}

void MoveHistory::reset() {
	current_index = 0;
	moves.clear();
}

int MoveHistory::getCurrentMoveIndex() {
	return current_index;
}

int MoveHistory::getTotalMoves() {
	return moves.size();
}

const std::vector<MoveEntry>& MoveHistory::getMoveHistory() const {
	return moves;
}

void MoveHistory::recordCastlingMove(const Move& move, const std::string& hash) {
	bool is_king_side = move.getType() == PositionType::KingSideCastle;
	Position rook_from(move.from.row, (is_king_side ? 7 : 0));
	Position rook_to(move.from.row, (is_king_side ? 5 : 3));
	Move rook_move(rook_from, rook_to);
	recordMove(MoveEntry(move, rook_move, hash));
}
