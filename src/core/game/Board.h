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

    void update(const float& dt, const Move* last_move);
    void render(sf::RenderWindow& window);

    void makeMove(const Move& move);
    void undoLastMove();

    std::unique_ptr<Piece>& getPieceAt(const Position& position);
    void setPieceAt(const Position& position, std::unique_ptr<Piece> new_piece);

    bool isKingChecked(const ChessColor& color, bool skipMoveValidation = false);

    std::vector<Position> getValidMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece);

    std::vector<Position> getValidPawnMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece);
    
    const std::vector<std::unique_ptr<Piece>>& getCapturedPieces(const ChessColor& color) const;

    bool isValidMove(const Position& position);
    bool isCastlingMove(const Position& position);
    const Piece* getSelectedPiece() const;

    void selectPiece(const Position& position);
    void unselectPiece();

    bool isWhiteSide() const;
    void flip();

    std::string computeHash();

    BoardGeometry& getGeometry();

private:
    void initializeBoard();

    void renderBoard(sf::RenderWindow& window);
    void renderTiles(sf::RenderWindow& window);
    void renderTileAt(sf::RenderWindow& window, const Position& position);
    void renderPieceAt(sf::RenderWindow& window, const Position& position);

    void updateTileStates(const float& dt);
    std::string getTileOverlayName(const Position& position);
    TileState computeTileState(const Position& position);

    bool isInBounds(const Position& position) const;
    bool hasFriendlyPiece(const Position& position, const ChessColor& color);
    bool hasKingAt(const Position& position);
    bool isPathObstructed(const Position& from, const Position& to);
    bool hasPieceAt(const Position& position);
    bool willExposeKing(const Position& from, const Position& to, const ChessColor& king_color);
    bool isValidPawnStep(const Position& from, const Position& to, const ChessColor& king_color);
    bool isValidPawnCapture(const Position& from, const Position& to, const ChessColor& king_color);

    Position getEnPassantMove(Piece* moving_piece);

    void makeCastlingMove(const Move & king_move);
    bool canCastle(const ChessColor& color, bool king_side);

    Position getKingPosition(const ChessColor& color);

    std::string computePlacementField();
    std::string computeRowPlacement(int row);
    char getPieceSymbol(const PieceType& type, const ChessColor& color);
    char getActiveColor();
    std::string getCastlingRights();

    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::vector<std::unique_ptr<Piece>> white_captured;
    std::vector<std::unique_ptr<Piece>> black_captured;

    Piece* selected_piece;
    std::vector<Position> valid_moves;
    const Move* last_move;

    std::array<std::array<TileState, 8>, 8> tile_states;

    bool is_white_side; // to track board orientation

    BoardGeometry geometry;
};