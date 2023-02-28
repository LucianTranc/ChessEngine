#include "ChessAI.hpp"
#include <stdlib.h>
#include <climits>

// base call for minimax algorithm. Needed so that we can keep track of 
// which move is being scored.
void ChessAI::getMove(int * start, int * end, Board * board, int ply)
{
    evaluations = 0;
    int bestScore = INT_MIN;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    int bestMoveStart = 0;
    int bestMoveEnd = 0;

    // get the pieces of the side whose turn it is
    U64 piecesToCheck = board->occupancy[board->turn];

    // copy of the board
    Board boardCopy = *board;

    while (piecesToCheck)
    {
        // copy of the board
        boardCopy = *board;

        // get the piece to check
        int piece = pop_LSB(piecesToCheck);

        // get the legal moves of the piece
        U64 legalMoves = boardCopy.getLegalMoves(piece);
        
        //std::cout<<"Piece: "<<(Square)piece<<", Legal Moves: "<<countSetBits(legalMoves)<<std::endl;

        // loop over the legal moves
        while (legalMoves)
        {
            // pop the move from the legal moves
            int move = pop_LSB(legalMoves);

            // make another copy of the board
            boardCopy = *board;

            // move the piece and switch the turn
            boardCopy.movePiece(piece, move);
            boardCopy.turn = boardCopy.turn ? white : black;

            int score = min(&boardCopy, ply - 1, alpha, beta);

            //std::cout<<"Move: "<<(Square)move<<", Score: "<<score<<std::endl;

            // record move if this move is the best move so far
            if (score > bestScore)
            {
                bestScore = score;
                bestMoveStart = piece;
                bestMoveEnd = move;
                //std::cout<<"Best move: "<<(Square)bestMoveStart<<", "<<(Square)bestMoveEnd<<std::endl;
            }
            // update alpha for the remaining branches
            if (score > alpha) alpha = score;
        }
    }

    *start = bestMoveStart;
    *end = bestMoveEnd;

}

// max function for minimax
int ChessAI::max(Board * board, int depth, int alpha, int beta)
{
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

    // if the board states are a white win or a black win, stop searching and give the 
    // maximum or minimum score depending on the situation
    if (board->getGameState() == whiteWin) return colour ? INT_MIN : INT_MAX;
    if (board->getGameState() == blackWin) return colour ? INT_MAX : INT_MIN;

    // if the bottom is reached evaluate the position
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

    if (board->getGameState() == whiteWin) return colour ? INT_MIN : INT_MAX;
    if (board->getGameState() == blackWin) return colour ? INT_MAX : INT_MIN;
    if (depth == 0) return evaluatePosition(board);

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

// get a random move, used for debugging
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

// evaluation function
int ChessAI::evaluatePosition(Board * board)
{
    int score = 0;
    int opponentScore = 0;
    Colour opponentColour = colour ? white : black;

    // each piece on the board is assigned a value
    score += countSetBits(board->pieces[colour][pawn]) * 100;
    score += countSetBits(board->pieces[colour][knight]) * 300;
    score += countSetBits(board->pieces[colour][bishop]) * 300;
    score += countSetBits(board->pieces[colour][rook]) * 500;
    score += countSetBits(board->pieces[colour][queen]) * 900;

    // for each type of piece, add the score based on their positions

    U64 pieces = board->pieces[colour][pawn];
    while (pieces)
    {
        score += pawnPositions[colour][pop_LSB(pieces)];
    }

    pieces = board->pieces[colour][knight];
    while (pieces)
    {
        score += knightPositions[pop_LSB(pieces)];
    }

    pieces = board->pieces[colour][bishop];
    while (pieces)
    {
        score += bishopPositions[pop_LSB(pieces)];
    }

    pieces = board->pieces[colour][rook];
    while (pieces)
    {
        score += rookPositions[pop_LSB(pieces)];
    }

    pieces = board->pieces[colour][queen];
    while (pieces)
    {
        score += queenPositions[pop_LSB(pieces)];
    }

    opponentScore += countSetBits(board->pieces[opponentColour][pawn]) * 100;
    opponentScore += countSetBits(board->pieces[opponentColour][knight]) * 300;
    opponentScore += countSetBits(board->pieces[opponentColour][bishop]) * 300;
    opponentScore += countSetBits(board->pieces[opponentColour][rook]) * 500;
    opponentScore += countSetBits(board->pieces[opponentColour][queen]) * 900;

    pieces = board->pieces[opponentColour][pawn];
    while (pieces)
    {
        opponentScore += pawnPositions[opponentColour][pop_LSB(pieces)];
    }

    pieces = board->pieces[opponentColour][knight];
    while (pieces)
    {
        opponentScore += knightPositions[pop_LSB(pieces)];
    }

    pieces = board->pieces[opponentColour][bishop];
    while (pieces)
    {
        opponentScore += bishopPositions[pop_LSB(pieces)];
    }

    pieces = board->pieces[opponentColour][rook];
    while (pieces)
    {
        opponentScore += rookPositions[pop_LSB(pieces)];
    }

    pieces = board->pieces[opponentColour][queen];
    while (pieces)
    {
        opponentScore += queenPositions[pop_LSB(pieces)];
    }

    evaluations++;

    return score - opponentScore;
}