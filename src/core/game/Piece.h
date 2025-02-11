#pragma once

#include "BoardUtilities.h"
#include "../GameObject.h"
#include "../managers/AssetManager.h"
#include "BoardGeometry.h"

enum class PieceType {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King
};

class Piece {
public:
    Piece(PieceType, ChessColor, Position);
    Piece(const Piece& other);

    void update(const float& dt);
    void render(sf::RenderWindow& window, const BoardGeometry& geometry);

    void setPosition(const Position new_position);
    std::unique_ptr<Piece> clone() const;
    Position getPosition();

    void incrementMoveCount();
    void decrementMoveCount();

    std::vector<Position> getPossibleMoves() const;

    bool hasMoved() const;
    bool canBeObstructed() const;
    PieceType getType() const;
    ChessColor getColor();
    Position getPosition() const;
    std::string getPieceCode();

private:
    std::vector<Position> getPawnPossibleMoves() const;
    std::vector<Position> getKnightPossibleMoves() const;
    std::vector<Position> getRookPossibleMoves() const;
    std::vector<Position> getBishopPossibleMoves() const;
    std::vector<Position> getQueenPossibleMoves() const;
    std::vector<Position> getKingPossibleMoves() const;

    std::vector<Position> getLinePossibleMoves(const int& dx, const int& dy) const;

    int move_count;
    Position current_position;
    sf::Sprite sprite;
    PieceType type;
    ChessColor color;
};

#define MERGE_VECTORS(vect1, vect2) vect1.insert(vect1.end(), vect2.begin(), vect2.end())