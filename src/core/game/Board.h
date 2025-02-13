#pragma once

#include "BoardGeometry.h"
#include "BoardUtilities.h"
#include "Piece.h"
#include "../GameObject.h"
#include <array>

class MoveHistory;

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

#pragma region Constructors and Destructor

    Board();
    Board(const std::string& board_hash);
    Board(const Board& other); 
    Board& operator=(const Board& other);
    ~Board();

#pragma endregion
    void update(const float& dt);
    void render(sf::RenderWindow& window);
    BoardGeometry& getGeometry();
    void flip();

#pragma region Movement

    void makeMove(const Move& move);
    void undoLastMove();

    void promotePawn(const Position& position, const PieceType& promotion_type);
    bool isLastMovePromotion();
    std::unique_ptr<Piece>& getPieceAt(const Position& position);
    void setPieceAt(const Position& position, std::unique_ptr<Piece> new_piece);

    bool isKingChecked(const ChessColor& color, bool skipMoveValidation = false);
    std::vector<Position> getValidMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece);
    std::vector<Position> getAllValidMoves(const ChessColor& color);
    std::vector<Position> getValidPawnMoves(const std::vector<Position>& candidate_moves, Piece* moving_piece);

    bool isValidMove(const Position& position);
    bool isCastlingMove(const Position& position);

    void selectPiece(const Position& position, const ChessColor& current_turn);
    void unselectPiece();
    Piece* getSelectedPiece();

#pragma endregion

    bool hasPieceAt(const Position& position);
    const std::vector<std::unique_ptr<Piece>>& getCapturedPieces(const ChessColor& color) const;
    bool isWhiteSide() const;

#pragma region FEN Hashing

    std::string computeHash(int turn_count);
    void loadFromHash(const std::string& board_hash);
    void renderHash(const std::string& board_hash, sf::RenderWindow& window);

#pragma endregion

private:
    void initializeBoard();
    void copy(const Board& other);
    void clear();

#pragma region Rendering Helpers

    void renderBoard(sf::RenderWindow& window);
    void renderTiles(sf::RenderWindow& window);
    void renderTileAt(sf::RenderWindow& window, const Position& position);
    void renderPieceAt(sf::RenderWindow& window, const Position& position);
    void updateTileStates(const float& dt);
    std::string getTileOverlayName(const Position& position);
    TileState computeTileState(const Position& position);

#pragma endregion

#pragma region Movement Helpers

    bool isInBounds(const Position& position) const;
    bool hasFriendlyPiece(const Position& position, const ChessColor& color);
    bool hasKingAt(const Position& position);
    bool isPathObstructed(const Position& from, const Position& to);
    bool willExposeKing(const Position& from, const Position& to, const ChessColor& king_color);
    bool isValidPawnStep(const Position& from, const Position& to, const ChessColor& king_color);
    bool isValidPawnCapture(const Position& from, const Position& to, const ChessColor& king_color);
    Position getEnPassantMove(Piece* moving_piece);
    void makeCastlingMove(const Move & king_move);
    bool canCastle(const ChessColor& color, bool king_side);
    Position getKingPosition(const ChessColor& color);

#pragma endregion

#pragma region FEN Hashing Helpers

    std::string computePlacementField();
    std::string computeRowPlacement(int row);
    char getPieceSymbol(const PieceType& type, const ChessColor& color);
    PieceType typeFromSymbol(const char& symbol);
    ChessColor colorFromSymbol(const char& symbol);
    char getActiveColor();
    std::string getCastlingRights();
    std::string getEnPassantTarget();
    std::string getAlgebraicNotation(const Position& position);

#pragma endregion

    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    std::vector<std::unique_ptr<Piece>> white_captured;
    std::vector<std::unique_ptr<Piece>> black_captured;

    Piece* selected_piece;
    std::vector<Position> valid_moves;
    Move* last_move;

    std::array<std::array<TileState, 8>, 8> tile_states;

    int halfmove_clock;
    BoardGeometry geometry;
};