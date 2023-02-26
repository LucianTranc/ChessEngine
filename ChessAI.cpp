#include "ChessAI.hpp"
#include <stdlib.h>
#include <climits>

void ChessAI::getMove(int * start, int * end, Board * board, int ply)
{
    // start with basic min max search

    // call depth first search (ply) moves deep

}

int ChessAI::max(Board * board, int depth)
{
    /* if ( depth == 0 ) return evaluate();
    int max = -oo;
    for ( all moves) {
        score = mini( depth - 1 );
        if( score > max )
            max = score;
    }
    return max; */

    if (depth == 0) return evaluatePosition(board);

    int max = INT_MIN;

    // loop through each move

    

}


void ChessAI::getRandomMove(int * start, int * end, Board * board)
{
    U64 piecesToConsider = board->occupancy[board->turn];
    U64 piecesToConsiderCopy = piecesToConsider;

    while (piecesToConsiderCopy)
    {
        int piece = pop_LSB(piecesToConsiderCopy);

        U64 legalMoves = board->getLegalMoves(piece);

        if (legalMoves == 0ULL)
        {
            clear_bit(piecesToConsider, piece);
        }
    }

    int pieceCount = countSetBits(piecesToConsider);

    printf("pieceCount: %d\n", pieceCount);

    srand(4039284930);

    int random = rand() % (pieceCount);

    printf("Random: %d\n", random);

    for (int i = 0; i < random; i++)
    {
        pop_LSB(piecesToConsider);
    }

    *start = pop_LSB(piecesToConsider);

    U64 legalMoves = board->getLegalMoves(*start);

    *end = pop_LSB(legalMoves);

    return;

}

// return positive value if white is winning
// return negative value if black is winning
int ChessAI::evaluatePosition(Board * board)
{
    int whiteScore = 0;
    int blackScore = 0;

    whiteScore += countSetBits(board->pieces[white][pawn]) * 1;
    whiteScore += countSetBits(board->pieces[white][knight]) * 3;
    whiteScore += countSetBits(board->pieces[white][bishop]) * 3;
    whiteScore += countSetBits(board->pieces[white][rook]) * 5;
    whiteScore += countSetBits(board->pieces[white][queen]) * 9;

    blackScore += countSetBits(board->pieces[black][pawn]) * 1;
    blackScore += countSetBits(board->pieces[black][knight]) * 3;
    blackScore += countSetBits(board->pieces[black][bishop]) * 3;
    blackScore += countSetBits(board->pieces[black][rook]) * 5;
    blackScore += countSetBits(board->pieces[black][queen]) * 9;

    return whiteScore - blackScore;
}