#include "Board.hpp"

class ChessAI
{
public:
    // https://www.chessprogramming.org/Minimax

    int max(Board * board, int depth);
    int min(Board * board, int depth);

    void getRandomMove(int * start, int * end, Board * board);
    void getMove(int * start, int * end, Board * board, int ply);
    void search(Board * board, int depth, int maxDepth);
    int evaluatePosition(Board * board);

};