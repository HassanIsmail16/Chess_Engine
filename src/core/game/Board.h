#pragma once

#include "BoardGeometry.h"
#include "BoardUtilities.h"
#include "Piece.h"
#include "../GameObject.h"
#include <array>

enum class TileState {
    None,
    Selected,
    Highlighted,
    Attacked,
    Special,
    LastMove
};

class Board {
public:
    Board();
    Board(const Board& other);

    void update(const float& dt);
    void render(sf::RenderWindow& window);

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
    std::vector<Position> getValidatedPawnMoves(std::vector<Position>& candidate_moves, Piece* moving_piece);

    void renderBoard(sf::RenderWindow& window);
    void renderTiles(sf::RenderWindow& window);
    void renderTileAt(sf::RenderWindow& window, const Position& position);
    void renderPieceAt(sf::RenderWindow& window, const Position& position);

    void updateTileStates(const float& dt);
    std::string getTileOverlayName(const Position& position);
    TileState computeTileState(const Position& position);

    bool isInBounds(const Position& position) const;
    bool hasFriendlyPiece(const Position& position, const ChessColor& color);
    bool willCaptureKing(const Position& position);
    bool isPathObstructed(const Position& from, const Position& to);
    bool hasPieceAt(const Position& position);
    bool willExposeKing(const Position& from, const Position& to, const ChessColor& color);

    Position getKingPosition(const ChessColor& color);

    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::vector<std::unique_ptr<Piece>> white_captured;
    std::vector<std::unique_ptr<Piece>> black_captured;

    Piece* selected_piece;
    std::vector<Position> valid_moves;
    Move* last_move;

    std::array<std::array<TileState, 8>, 8> tile_states;

    bool is_white_side; // to track board orientation

    BoardGeometry geometry;
};