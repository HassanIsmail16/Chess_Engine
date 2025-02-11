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
    Position(const int& row, const int& col, const PositionType& type = PositionType::Regular): row(row), col(col), type(type) {}
    bool operator==(const Position& other) { return row == other.row && col == other.col; }
    int row;
    int col;
    PositionType type;
};

struct Move {
    Move(const Position& from, const Position& to, Piece* taken_over): from(from), to(to), taken_over(taken_over) {}
    const Position from;
    const Position to;
    Piece* taken_over;
};