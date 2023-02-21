#include <iostream>
#include <bitset>
#include <string>
#include <string.h>
#include <cctype>
#include <ctype.h>

typedef u_int64_t U64;

// hex value representing the ranks
const U64 FILE_A = 0x0101010101010101ULL;
const U64 FILE_B = FILE_A << 1;
const U64 FILE_C = FILE_A << 2;
const U64 FILE_D = FILE_A << 3;
const U64 FILE_E = FILE_A << 4;
const U64 FILE_F = FILE_A << 5;
const U64 FILE_G = FILE_A << 6;
const U64 FILE_H = FILE_A << 7;

const U64 RANK_1 = 0xFFULL;
const U64 RANK_2 = RANK_1 << 8;
const U64 RANK_3 = RANK_1 << 16;
const U64 RANK_4 = RANK_1 << 24;
const U64 RANK_5 = RANK_1 << 32;
const U64 RANK_6 = RANK_1 << 40;
const U64 RANK_7 = RANK_1 << 48;
const U64 RANK_8 = RANK_1 << 56;


#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))  
#define clear_bit(b, i) ((b) &= ~(1ULL << i)) 
#define get_LSB(b) (__builtin_ctzll(b))

// square encoding
enum {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

/*
constexpr U64 west(U64 b) { return (b & ~FILE_A) << 1; }
constexpr U64 east(U64 b) { return (b & ~FILE_H) >> 1; }
constexpr U64 north_west(U64 b) { return (b & ~FILE_A) << 9; }
constexpr U64 north_east(U64 b) { return (b & ~FILE_H) << 7; }
*/

inline int pop_LSB(U64 &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}

struct board
{
    U64 allPieces;
    U64 whitePieces;
    U64 blackPieces;
    U64 whitePawns;
    U64 blackPawns;
    bool whiteToMove;
} 
typedef board;

struct position
{
    char file;
    int rank;
} 
typedef position;

void printBoard(board board);
void printBoardWhiteDown(board board);
void printBitBoard(U64 bitboard);
int getPositionIndex(position pos);
void movePiece(board * board, int start, int end);
bool validateMove(board * board, position start, position end);
U64 getWhitePawnMoves(board * board, int p);
U64 getBlackPawnMoves(board * board, int p);
U64 maskBishopAttacks(int square);
U64 maskRookAttacks(int square);

bool running = true;

int main()
{
    std::cout<<"Tranc Engine"<<std::endl;

    // define U64 variables and print them as boards

    U64 whitePawns = RANK_2;
    U64 blackPawns = RANK_7;
    U64 whitePeices = whitePawns;
    U64 blackPeices = blackPawns;

    board testBoard = {whitePeices | blackPeices, whitePeices, blackPeices, whitePawns, blackPawns, true};

    printBoard(testBoard);

    std::string move;

    while (running)
    {
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

        position moveStart = {static_cast<char>(islower(move[0])?toupper(move[0]):move[0]), move[1] - '0'};
        position moveEnd = {static_cast<char>(islower(move[3])?toupper(move[3]):move[3]), move[4] - '0'};

        // std::cout<<moveStart.file<<moveStart.rank<<std::endl;
        // std::cout<<moveEnd.file<<moveEnd.rank<<std::endl;

        // valid move?

        if (validateMove(&testBoard, moveStart, moveEnd))
        {
            printf("valid move\n");
            movePiece(&testBoard, getPositionIndex(moveStart), getPositionIndex(moveEnd));
            testBoard.whiteToMove = !testBoard.whiteToMove;
        }
        else
        {
            printf("invalid move\n");
        }
        

        // move peice

        printBoard(testBoard);

    }


    return 0;
}

void printBoard(board board)
{
    if (board.whiteToMove)
    {
        printf("\nWhite to move\n");
    }
    else
    {
        printf("\nBlack to move\n");
    }
    for (int i = 0; i < 64; i++)
    {
        if (i%8 == 0) printf("%d  ", i/8 + 1);
        std::string printString = ".";

        if (((board.whitePawns >> i) & 1) == 1)
        {
            printString = "wP";
        }
        else if (((board.blackPawns >> i) & 1) == 1)
        {
            printString = "bP";
        }

        printf("%-3s", printString.c_str());

        if ((i+1)%8 == 0) printf("\n");

    }
    printf("   A  B  C  D  E  F  G  H\n");
    return;
}

void printBoardWhiteDown(board board)
{
    std::string boardToPrint[64];

    for (int i = 63; i >= 0; i--)
    {
        if (((board.whitePawns >> i) & 1) == 1)
        {
            boardToPrint[i] = "wP";
        }
        else if (((board.blackPawns >> i) & 1) == 1)
        {
            boardToPrint[i] = "bP";
        }
        else
        {
            boardToPrint[i] = '.';
        }
    }

    for (int i = 7; i >= 0; i--)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%s\t", boardToPrint[(i*8)+j].c_str());
        }
        printf("\n\n");
    }

    return;
}

void printBitBoard(U64 bitboard)
{
    for (int i = 0; i < 64; i++)
    {
        if (i%8 == 0) printf("%d  ", (i/8) + 1);

        printf("%d ", get_bit(bitboard, i) ? 1 : 0);

        if ((i+1)%8 == 0) printf("\n");
    }
    printf("\n   a b c d e f g h\n\n");
    return;
}

int getPositionIndex(position pos)
{
    return pos.file - 'A' + ((pos.rank - 1) * 8);
}

void movePiece(board * board, int start, int end)
{
    if (get_bit(board->whitePawns, start))
    {
        clear_bit(board->whitePawns, start);
        set_bit(board->whitePawns, end);

        printf("moved white pawn\n");
    }
    else if (get_bit(board->blackPawns, start))
    {
        clear_bit(board->blackPawns, start);
        set_bit(board->blackPawns, end);
        printf("moved black pawn\n");
    }

    board->allPieces = board->whitePawns | board->blackPawns;

}

bool validateMove(board * board, position start, position end)
{

    // check if position selected contains a piece correspending with board->whiteToMove
    // find which peice is moving, call a method that calculates the possible allowed moves for
    // that peice type
    if (get_bit(board->whitePawns, getPositionIndex(start)) && board->whiteToMove)
    {
        U64 pawnMoves = getWhitePawnMoves(board, getPositionIndex(start));

        printf("White Pawn moves: \n");
        printBitBoard(pawnMoves);

        if (get_bit(pawnMoves, getPositionIndex(end)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (get_bit(board->blackPawns, getPositionIndex(start)) && !board->whiteToMove)
    {
        U64 pawnMoves = getBlackPawnMoves(board, getPositionIndex(start));

        printf("Black Pawn moves: \n");
        printBitBoard(pawnMoves);

        if (get_bit(pawnMoves, getPositionIndex(end)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

// get all possible pawn moves
U64 getWhitePawnMoves(board * board, int p)
{
    U64 pawnMoves = 0ULL;

    // if the pawn is on the second rank, it can move forward 2 spots

    printf("Getting white pawn moves:\n");

    printf("All pieces: \n");

    printBitBoard(board->allPieces);

    if (get_bit(board->blackPawns, p+9))
    {
        set_bit(pawnMoves, p+9);
    }

    if (get_bit(board->blackPawns, p+7))
    {
        set_bit(pawnMoves, p+7);
    }

    if (!get_bit(board->allPieces, p+8))
    {
        set_bit(pawnMoves, p+8);
    }

    if (get_bit(RANK_2, p) && !get_bit(board->allPieces, p+16))
    {
        set_bit(pawnMoves, p+16);
    }


    return pawnMoves;
}

U64 getBlackPawnMoves(board * board, int p)
{
    U64 pawnMoves = 0ULL;

    if (get_bit(board->whitePawns, p-9))
    {
        set_bit(pawnMoves, p-9);
    }

    if (get_bit(board->whitePawns, p-7))
    {
        set_bit(pawnMoves, p-7);
    }

    if (!get_bit(board->allPieces, p-8))
    {
        set_bit(pawnMoves, p-8);
    }
    if (get_bit(RANK_7, p) && !get_bit(board->allPieces, p-16))
    {
        set_bit(pawnMoves, p-16);
    }
    return pawnMoves;
}

// mask bishop attacks
// https://github.com/maksimKorzh/chess_programming/blob/master/src/magics/magics.c
U64 maskBishopAttacks(int square)
{
    // attack bitboard
    U64 attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
}

// https://github.com/maksimKorzh/chess_programming/blob/master/src/magics/magics.c
U64 maskRookAttacks(int square)
{
    // attacks bitboard
    U64 attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    // generate attacks
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
    
    // return attack map for bishop on a given square
    return attacks;
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


*/