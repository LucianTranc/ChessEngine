#include "Board.hpp"

class ChessAI
{
public:

    Colour colour;
    int evaluations = 0;
    
    ChessAI(Colour c) : colour(c) {}

    int max(Board * board, int depth, int alpha, int beta);
    int min(Board * board, int depth, int alpha, int beta);

    void getRandomMove(int * start, int * end, Board * board);
    void getMove(int * start, int * end, Board * board, int ply);
    void search(Board * board, int depth, int maxDepth);
    int evaluatePosition(Board * board);

private:

    // position tables are a modified version of the tables sourced from: 
    // https://github.com/SebLague/Chess-AI/blob/main/Assets/Scripts/Core/PieceSquareTable.cs

    // White pawns and black oawns have the same tables only reversed
    // d4, d5, e4 and e5 are all given a high value while d2, e2, d7 and e7 are given negative
    // values. This is done to discurage the AI from not moving their king and queen pawns at the
    // start of  the game. In chess, gaining control of the center of the board increases your chance
    // of winning. Furthermore, as the pawns rise through the ranks, the positional scores get higher
    // and higher. This is done to incentivise the AI to advance its pawns in hopes of a queen promotion.
    // The last row isn't given any score since the pawn will be a queen when it is in that position
    // and will get a +900 score from that move anyways.
    const int pawnPositions[2][64] = {{
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-20,-20, 10, 10,  5,
        5, -5,-10,  0,  0,-10, -5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    }}; 

    // knight moves favour center moves. This is because if a knight is near the edge of the board
    // some of its moves will be out of bounds which deacreases the amount of squares the knight as influence over
    // If the Knight is in the center of the board, it can move in all 8 directions.
    const int knightPositions[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };

    // Bishops are given negative scores when on the edge of the board since it can influence 
    // less squares and it also is prone to being trapped. 
    // bishops are encouraged to take on positions that attack the long diagonals
    const int bishopPositions[2][64] = {{
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    },
    {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    }};

    // rooks are encouraged to move to the center of their home rank
    // this adds incentive to castle and also attacks the center of the board
	const int rookPositions[2][64] = {{
         0,  0,  5, 10, 10,  5,  0,  0
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         5,  0,  0,  0,  0,  0,  0,  5,
         0,  0,  0,  0,  0,  0,  0,  0,
    },
    {
         0,  0,  0,  0,  0,  0,  0,  0,
         5,  0,  0,  0,  0,  0,  0,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  5, 10, 10,  5,  0,  0
    }};

    // queens are encouraged to be in the center of the board to have greater influence over the board
	const int queenPositions[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
         -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

};