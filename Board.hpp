#pragma once
#include "MoveGenerator.hpp"

class Board
{

public:
    // a bitboard for every type of piece of every colour
    // first index is the colour, second is the type
    U64 pieces[2][6];

    // a bitboard for all the pieces of a certain colour
    // index is the colour
    U64 occupancy[2];

    // all the pieces on the board
    U64 totalOccupancy;
    
    // the pinned pieces
    U64 pinnedPieces;

    Colour turn;

    // information needed for castling
    bool a1Moved = false;
    bool h1Moved = false;
    bool a8Moved = false;
    bool h8Moved = false;
    bool whiteKingMoved = false;
    bool blackKingMoved = false;

    void printBoard();
    void printBoardUnicode();
    void movePiece(int start, int end, bool calculatePins = true);
    void deletePiece(int position);
    int getPieceColour(int p);
    int getPieceType(int p);
    U64 getLegalMoves(int p);
    U64 getAttackers(int target, int attackingColour);
    GameState getGameState();

};