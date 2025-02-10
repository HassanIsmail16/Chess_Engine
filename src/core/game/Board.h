#pragma once

#include "BoardUtilities.h"
#include "../GameObject.h"
#include <array>

class Board : public GameObject {
public:
    Board();

    void update(const float& dt) override;
    void render(sf::RenderWindow& window) override;

    void makeMove(const Move& move);
    void undoLastMove();

    std::unique_ptr<Piece>& getPieceAt(const Position& position);
    void setPieceAt(const Position& position, std::unique_ptr<Piece> new_piece);

    bool isKingChecked(const ChessColor& color);

    const std::vector<std::unique_ptr<Piece>>& getCapturedPieces(const ChessColor& color) const;

    void selectPiece(const Position& position);
    void unselectPiece();

    bool isWhiteSide() const;
    void flip();

    uint64_t computeHash() const;

private:
    void initializeBoard();

    std::vector<Position> getValidMoves(std::vector<Position>& candidate_moves, Piece* moving_piece);

    void renderBoard(sf::RenderWindow& window);
    void renderTiles(sf::RenderWindow& window);
    void renderPieces(sf::RenderWindow& window);

    bool isInBounds(const Position& position) const;

    Position getKingPosition(const ChessColor& color);

    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::vector<std::unique_ptr<Piece>> white_captured;
    std::vector<std::unique_ptr<Piece>> black_captured;

    Piece* selected_piece;
    std::vector<Position> valid_moves;
    Move* last_move;

    bool is_white_side; // to track board orientation
};