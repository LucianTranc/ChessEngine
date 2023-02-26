#include "ChessAI.hpp"
#include <stdlib.h>
#include <climits>

void ChessAI::getMove(int * start, int * end, Board * board, int ply)
{
    // start with basic min max search

    // call depth first search (ply) moves deep
    evaluations = 0;
    int bestScore = INT_MIN;
    int bestMoveStart = 0;
    int bestMoveEnd = 0;

    U64 piecesToCheck = board->occupancy[board->turn];

    Board boardCopy = *board;

    while (piecesToCheck)
    {
        boardCopy = *board;

        int piece = pop_LSB(piecesToCheck);

        U64 legalMoves = boardCopy.getLegalMoves(piece);
        std::cout<<"Piece: "<<(Square)piece<<", Legal Moves: "<<countSetBits(legalMoves)<<std::endl;

        while (legalMoves)
        {
            int move = pop_LSB(legalMoves);

            boardCopy = *board;
            boardCopy.movePiece(piece, move);
            boardCopy.turn = boardCopy.turn ? white : black;

            int score = min(&boardCopy, ply - 1, INT_MIN, INT_MAX);

            std::cout<<"Move: "<<(Square)move<<", Score: "<<score<<std::endl;

            if (score > bestScore)
            {
                bestScore = score;
                bestMoveStart = piece;
                bestMoveEnd = move;
                std::cout<<"Best move: "<<(Square)bestMoveStart<<", "<<(Square)bestMoveEnd<<std::endl;
            }
        }
    }

    *start = bestMoveStart;
    *end = bestMoveEnd;

}

int ChessAI::max(Board * board, int depth, int alpha, int beta)
{
    //printf("max depth: %d\n", depth);
    // https://www.chessprogramming.org/Minimax
    /*
    if ( depth == 0 ) return evaluate();
    int max = -oo;
    for ( all moves) {
        score = mini( depth - 1 );
        if( score > max )
            max = score;
    }
    return max; */

    if (depth == 0) return evaluatePosition(board);

    int max = INT_MIN;

    U64 piecesToCheck = board->occupancy[board->turn];

    Board boardCopy = *board;

    while (piecesToCheck)
    {
        boardCopy = *board;

        int piece = pop_LSB(piecesToCheck);

        U64 legalMoves = boardCopy.getLegalMoves(piece);

        while (legalMoves)
        {
            int move = pop_LSB(legalMoves);

            boardCopy = *board;
            boardCopy.movePiece(piece, move);
            boardCopy.turn = boardCopy.turn ? white : black;

            int score = min(&boardCopy, depth - 1, alpha, beta);

            if (score > max) max = score;
            if (score > alpha) alpha = score;
            if (beta <= alpha) break;
        }
    }

    return max;
    
}

int ChessAI::min(Board * board, int depth, int alpha, int beta)
{
    //printf("min depth: %d\n", depth);

    /*
    if ( depth == 0 ) return -evaluate();
    int min = +oo;
    for ( all moves) {
        score = maxi( depth - 1 );
        if( score < min )
            min = score;
    }
    return min;
    */

    if (depth == 0) return -evaluatePosition(board);

    int min = INT_MAX;

    U64 piecesToCheck = board->occupancy[board->turn];

    Board boardCopy = *board;

    while (piecesToCheck)
    {
        boardCopy = *board;

        int piece = pop_LSB(piecesToCheck);

        U64 legalMoves = boardCopy.getLegalMoves(piece);

        while (legalMoves)
        {
            int move = pop_LSB(legalMoves);

            boardCopy = *board;
            boardCopy.movePiece(piece, move);
            boardCopy.turn = boardCopy.turn ? white : black;

            int score = max(&boardCopy, depth - 1, alpha, beta);

            if (score < min) min = score;
            if (score < beta) beta = score;
            if (beta <= alpha) break;
        }
    }

    return min;
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
    int score = 0;
    int opponentScore = 0;

    score += countSetBits(board->pieces[colour][pawn]) * 1;
    score += countSetBits(board->pieces[colour][knight]) * 3;
    score += countSetBits(board->pieces[colour][bishop]) * 3;
    score += countSetBits(board->pieces[colour][rook]) * 5;
    score += countSetBits(board->pieces[colour][queen]) * 9;

    opponentScore += countSetBits(board->pieces[colour ? white : black][pawn]) * 1;
    opponentScore += countSetBits(board->pieces[colour ? white : black][knight]) * 3;
    opponentScore += countSetBits(board->pieces[colour ? white : black][bishop]) * 3;
    opponentScore += countSetBits(board->pieces[colour ? white : black][rook]) * 5;
    opponentScore += countSetBits(board->pieces[colour ? white : black][queen]) * 9;

    evaluations++;

    return score - opponentScore;
}