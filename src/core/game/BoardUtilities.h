#pragma once

class Piece;

enum class ChessColor {
    White,
    Black
};

enum class PositionType {
    Regular,
    KingSideCastle,
    QueenSideCastle,
    EnPassant
};

struct Position {
    Position(const int& row = -1, const int& col = -1, const PositionType& type = PositionType::Regular): row(row), col(col), type(type) {}
    bool operator==(const Position& other) { return row == other.row && col == other.col; }
    bool operator==(const Position& other) const { return row == other.row && col == other.col; }
    int row;
    int col;
    PositionType type;
};

struct Move {
    Move() : from(-1, -1), to(-1, -1), taken_over(nullptr) {}
    Move(const Position& from, const Position& to, Piece* taken_over = nullptr): from(from), to(to), taken_over(taken_over) {}
    PositionType getType() const { return to.type; }
    bool isCastling() const { return to.type == PositionType::KingSideCastle || to.type == PositionType::QueenSideCastle;}
    bool isEnPassant() const { return to.type == PositionType::EnPassant; }
    bool isValid() const { return from != Position(-1, -1) && to != Position(-1 - 1); }
    Position from;
    Position to;
    Piece* taken_over;
};