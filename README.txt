Lucian Tranc
1045249

If the TA is reading this, I got premission from the professor to write my assignment in C++.
He said he might need to grade it himself. 

Commands for execution:
make
./play

I used: Apple clang version 14.0.0 (clang-1400.0.29.202)
However it should work with any recent version of g++
If this doesn't work for you locally you can run it on the school linux servers

The search algorithm currently searches 4 moves deep. It can also handle 5 moves deep relativley quickly.
If you want to try it you can edit the "PLY" macro defined in main.cpp on line 9.

I also made a print function that displays unicode chess pieces. It's easier on the eyes compared to
the assignment format so if you want to use that you can change the "PRINTFUNCTION" macro definied in main.cpp
on line 12 to printBoardUnicode()

Evaluation Function: 

I assigned a value to each piece on the board (but not the king):
pawn: 100
knight: 300
bishop: 300
rook: 500
queen: 900

After that I assign a score to the position of each piece on the board based on a 
precomputed table of values.

Pawns:
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0


Knights:
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50

Bishops:
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20

Rook:
     0,  0,  0,  0,  0,  0,  0,  0,
     5,  0,  0,  0,  0,  0,  0,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  5, 10, 10,  5,  0,  0

Queens:
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
