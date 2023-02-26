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

};