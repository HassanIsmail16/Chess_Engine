#pragma once

class Piece;

enum class ChessColor {
    White,
    Black
};

struct Position {
    Position(const int& row, const int& col): row(row), col(col) {}
    int row;
    int col;
};

enum class MoveType {
    Regular,
    KingSideCastle,
    QueenSideCastle,
    EnPassant
};

struct Move {
    Move(const Position& from, const Position& to, Piece* taken_over, const MoveType& type = MoveType::Regular): from(from), to(to), taken_over(taken_over), type(type) {}
    const Position from;
    const Position to;
    Piece* taken_over;
    const MoveType type;
};