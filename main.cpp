#include <bitset>
#include <string>
#include <string.h>
#include <cctype>
#include <ctype.h>
#include "Board.hpp"

bool running = true;

int main()
{
    std::cout<<"Tranc Engine"<<std::endl;

    Board testBoard;
    
    for (int i = 0; i < 6; i++)
    {
        testBoard.pieces[0][i] = 0ULL;
        testBoard.pieces[1][i] = 0ULL;
    }
    testBoard.occupancy[white] = 0ULL;
    testBoard.occupancy[black] = 0ULL;
    testBoard.totalOccupancy = 0ULL;
    testBoard.pinnedPieces = 0ULL;
    testBoard.turn = Colour::white;

    testBoard.pieces[white][pawn] = RANK_2;
    set_bit(testBoard.pieces[white][bishop], C1);
    set_bit(testBoard.pieces[white][bishop], F1);
    set_bit(testBoard.pieces[white][knight], B1);
    set_bit(testBoard.pieces[white][knight], G1);
    set_bit(testBoard.pieces[white][rook], A1);
    set_bit(testBoard.pieces[white][rook], H1);
    set_bit(testBoard.pieces[white][queen], D1);
    set_bit(testBoard.pieces[white][king], E1);

    testBoard.pieces[1][pawn] = RANK_7;
    set_bit(testBoard.pieces[black][bishop], C8);
    set_bit(testBoard.pieces[black][bishop], F8);
    set_bit(testBoard.pieces[black][knight], B8);
    set_bit(testBoard.pieces[black][knight], G8);
    set_bit(testBoard.pieces[black][rook], A8);
    set_bit(testBoard.pieces[black][rook], H8);
    set_bit(testBoard.pieces[black][queen], D8);
    set_bit(testBoard.pieces[black][king], E8);

    for (int i = 0; i < 6; i++)
    {
        testBoard.occupancy[white] |= testBoard.pieces[white][i];
    }

    for (int i = 0; i < 6; i++)
    {
        testBoard.occupancy[black] |= testBoard.pieces[black][i];
    }

    testBoard.totalOccupancy = testBoard.occupancy[white] | testBoard.occupancy[black];

    std::string move;
    while (running)
    {
        testBoard.printBoard();

        std::cout << "enter move: ";
        std::getline(std::cin, move);

        // check if move is valid
        if (strlen(move.c_str()) != 5
            || !((move[0] >= 'a' && move[0] <= 'h') || (move[0] >= 'A' || move[0] <= 'H'))
            || (isdigit(move[1]) == 0 || move[1] == '0' || move[1] == '9')
            || !((move[3] >= 'a' && move[3] <= 'h') || (move[3] >= 'A' || move[3] <= 'H'))
            || (isdigit(move[4]) == 0 || move[4] == '0' || move[4] == '9') || move[2] != ' ')
        {
            std::cout << "invalid move" << std::endl;
            continue;
        }

        int moveStart = ((move[1] - '0' - 1) * 8) + (islower(move[0]) ? toupper(move[0]) : move[0]) - 'A';
        int moveEnd = ((move[4] - '0' - 1) * 8) + (islower(move[3]) ? toupper(move[3]) : move[3]) - 'A';

        // valid move?
        // calculate all the legal moves of the piece selected
        
        U64 legalMoves = testBoard.getLegalMoves(moveStart);

        // if the move end is not in the legal moves then invalid
        if (!get_bit(legalMoves, moveEnd))
        {
            printf("invalid move, not a legal move");
            continue;
        }
        
        // move piece
        printf("move piece\n");
        testBoard.movePiece(moveStart, moveEnd);
        testBoard.turn = testBoard.turn ? Colour::white : Colour::black;

        GameState state = testBoard.getGameState();
        
        switch (state)
        {
            case whiteWin:
                printf("Checkmate, White wins\n");
                return 0;
            case blackWin:
                printf("Checkmate, Black wins\n");
                return 0;
            case stalemate:
                printf("Stalemate\n");
                return 0;
        }

    }

    return 0;
}


/*
---------------------  Magic bitboards notes  -------------------------------------
Sources: https://www.chessprogramming.org/Magic_Bitboards 
-----------------------------------------------------------------------------------

Magic bitboards is a technique used to quickly find rook and bishop attacks.

Rook and bishop attacks are harder to find since they are sliding peices. Unlike jumping peices like
the knight who can basically teleport to a square, sliding peices can be blocked by other peices. So
finding where a sliding peice can move to is dependent on the position of the players other peices, and
the position of the opponents peices.

Magic bitboards is a technique that uses prebuilt lookup tables to get the attack squares of a sliding peice
at any given square. We use the position of the peice and a bitboard containing all occupied spaces on the
board as a key to a dictionary that stores the attacking squares in every situation.

We calculate a bitboard with all the blocking peices by doing the bitwise operation:
(all occupied squares) & (squares the peice can slide to).

We multiply the bitboard with all blocking peices by some magic number that gives us
an index that is a smaller number than just the number value the bitboard
with the blocking peices would be on its own.
 
                                                               any consecutive
| relevant occupancy  bitboard with   |                       combination of
| rook d4, 10 bits    occupied squares|                       the masked bits
| . . . . . . . .     . . . . . . . . |    . . . . . . . .     4 5 6 B C E F G]
| . . . 6 . . . .     . . . . . . . . |    . . .some . . .     . . . . . .[1 2
| . . . 5 . . . .     . 1 . 1 . . . . |    . . . . . . . .     . . . . . . . .
| . . . 4 . . . .     . . . . . . . . |    . . .magic. . .     . . . . . . . .
| . B C . E F G .  &  . . . 1 1 . . . | *  . . . . . . . .  =  . . garbage . .    >> (64-10)
| . . . 2 . . . .     . . . . . . . . |    . . .bits . . .     . . . . . . . .
| . . . 1 . . . .     . 1 . . . . 1 . |    . . . . . . . .     . . . . . . . .
| . . . . . . . .     . . . . . . . . |    . . . . . . . .     . . . . . . . .
|                                     |

So if you want to get a bishop attack at some square s, you could do:

bishopAttacks [s][i];

where i is the index that was calculated using the blocking squares times the magic number.

U64 bishopAttacks[64] [512]; // 256 K
U64 rookAttacks  [64][4096]; // 2048K

struct SMagic {
   U64 mask;  // to mask relevant squares of both lines (no outer squares)
   U64 magic; // magic 64-bit factor
};

SMagic bishopTbl[64];
SMagic rookTbl  [64];

U64 bishopAttacks(U64 occ, enumSquare sq) {
   occ &= bishopTbl[sq].mask;
   occ *= bishopTbl[sq].magic;
   occ >>= 64-9;
   return bishopAttacks[sq][occ];
}

64 squares where the bishop can be, times 512 possible combinations the relevant squares can be in for the bishop
64 squares where the rook can be, times 4048 possible combinations the relevant squares can be in for the rook

Generating the tables:

First you need to generate the magic tables that store the mask and the magic. (the mask is the
squares the sliding peices can slide to and the magic is the random number we need to find)

For each square on the board, generate the mask for the given sliding peice,

Then for generating the magic you can just use trial and error.

Next steps:
1. Write your own attack and mask functions for rooks and bishops.
2. Make the game work with rooks and bishops
3. Add Knights
4. Add King
5. Add Queen (Bishop + Rook)
6. Test all the rules and make sure game works
7. Do AI
8. If Basic AI works then add the bishop and knight attack lookup tables

For bishop and knight lookup attack lookup tables:
1. Generate the magic numbers for the rook and bishop using:
    https://www.chessprogramming.org/Looking_for_Magics#:~:text=Magic%20Bitboards%20is%20probably%20the,of%20a%20bishop%20or%20rook. 
        - Write your own attack and mask functions for rooks and bishops
        - Copy the rest of the code and translate it to C++
*/