#include "GameStatusManager.h"
#include "../log/Logger.hpp"

GameStatusManager::GameStatusManager() {
	current_turn = ChessColor::White;
	turn_count = 0;
	repetition_tracker.clear();
}

bool GameStatusManager::isCheckMate(Board& board, const ChessColor& color) {
	if (!board.isKingChecked(color, true)) {
		return false;
	} // king is not in check

	return board.getAllValidMoves(color).empty();
}

bool GameStatusManager::isDraw(Board& board) {
	return isStaleMate(board) || isInsufficientMaterials(board) || isThreeFoldRepetition(board);
}

bool GameStatusManager::isInsufficientMaterials(Board& board) {
	// count materials
	int white_pieces = 0, black_pieces = 0;
	int white_bishops = 0, black_bishops = 0;
	int white_knights = 0, black_knights = 0;

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			Position current_position(row, col);
			
			if (!board.hasPieceAt(current_position)) {
				continue;
			} // current position is empty

			auto& current_piece = board.getPieceAt(current_position);

			if (current_piece->getType() == PieceType::King) {
				continue;
			} // current piece is a king (kings are not counted as materials)
			
			if (current_piece->getColor() == ChessColor::White) {
				white_pieces++;
				white_bishops += (current_piece->getType() == PieceType::Bishop);
				white_knights += (current_piece->getType() == PieceType::Knight);
			} else {
				black_pieces++;
				black_bishops += (current_piece->getType() == PieceType::Bishop);
				black_knights += (current_piece->getType() == PieceType::Knight);
			}
		}
	}

	if (white_pieces == 0 && black_pieces == 0) {
		LOG_INFO("INSUFFICIENT MATERIALS");
		return true;
	} // king vs king

	if ((white_pieces == 1 && (white_knights == 1 || white_bishops == 1)) 
		&& black_pieces == 1 && (black_knights == 1 || black_bishops == 1)) {
		LOG_INFO("INSUFFICIENT MATERIALS");
		return true;
	} 

	return false;
}

bool GameStatusManager::isThreeFoldRepetition(Board& board) {
	for (const auto& entry : repetition_tracker) {
		if (entry.second == 3) {
			LOG_INFO("THREEFOLD REPETITIOM");
			return true;
		}
	}
	
	return false;
}

bool GameStatusManager::isStaleMate(Board& board) {
	return isStaleMate(board, ChessColor::White) || isStaleMate(board, ChessColor::Black);
}

bool GameStatusManager::isStaleMate(Board& board, const ChessColor& color) {
	if (board.isKingChecked(color)) {
		return false;
	} // king is in check

	return board.getAllValidMoves(color).empty();
}

void GameStatusManager::registerBoard(const std::string& board_hash) {
	repetition_tracker[board_hash]++;
}

void GameStatusManager::unregisterBoard(const std::string& board_hash) {
	repetition_tracker[board_hash]--;
}

void GameStatusManager::nextTurn() {
	current_turn = (current_turn == ChessColor::White ? ChessColor::Black : ChessColor::White);
	turn_count++;
}

void GameStatusManager::previousTurn() {
	current_turn = (current_turn == ChessColor::White ? ChessColor::Black : ChessColor::White);
	turn_count--;
}

ChessColor GameStatusManager::getCurrentTurn() const {
	return current_turn;
}

void GameStatusManager::endTurn(Board& board) {
	if (isCheckMate(board, ChessColor::White)) {
		LOG_INFO("BLACK WINS");
		// TODO: push checkmate event
		return;
	}
	
	if (isCheckMate(board, ChessColor::Black)) {
		LOG_INFO("WHITE WINS");
		// TODO: push checkmate event
		return;
	}

	if (isDraw(board)) {
		LOG_INFO("DRAW");
		// TODO: push draw event
		return;
	}

	nextTurn();
}

int GameStatusManager::getTurnCount() const {
	return turn_count;
}

std::string GameStatusManager::cleanUpHash(const std::string& board_hash) {
	// TODO: implement
	return board_hash;
}
