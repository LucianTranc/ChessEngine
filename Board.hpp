#pragma once
#include "MoveGenerator.hpp"

class Board
{

public:
    U64 pieces[2][6];
    U64 occupancy[2];
    U64 totalOccupancy;
    U64 pinnedPieces;
    Colour turn;

    bool a1Moved = false;
    bool h1Moved = false;
    bool a8Moved = false;
    bool h8Moved = false;
    bool whiteKingMoved = false;
    bool blackKingMoved = false;

    void printBoard();
    void movePiece(int start, int end, bool calculatePins = true);
    void deletePiece(int position);
    int getPieceColour(int p);
    int getPieceType(int p);
    U64 getLegalMoves(int p);
    U64 getAttackers(int target, int attackingColour);
    GameState getGameState();

};