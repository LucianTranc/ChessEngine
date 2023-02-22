#include <iostream>
#include <bitset>
#include <string>
#include <string.h>
#include <cctype>
#include <ctype.h>

typedef u_int64_t U64;

// hex value representing the files and ranks
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
enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

// overloading << operator to print Square enum names
std::ostream &operator << (std::ostream& stream, Square s)
{
    const std::string squareNames[] = {
        "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
        "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
        "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
        "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
        "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
        "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
        "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
        "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8"
    };
   return stream << squareNames[s];
}

inline int pop_LSB(U64 &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}

enum Piece{
    pawn, knight, bishop, rook, queen, king
};

// overloading << operator to print Piece enum names
std::ostream &operator << (std::ostream& stream, Piece s)
{
    const std::string squareNames[] = {
        "P", "N", "B", "R", "Q", "K"
    };
   return stream << squareNames[s];
}

struct board
{
    U64 white[6];
    U64 black[6];
    U64 whiteOccupancy;
    U64 blackOccupancy;
    U64 occupancy;
    bool whiteToMove;
} 
typedef board;

void printBoard(board * board);
void printBitBoard(U64 bitboard);
void movePiece(int start, int end, board * board);
U64 getPawnAttacks(int square, U64 occupancy, bool isWhite);
U64 getRookAttacks(int square, U64 occupancy);
U64 getBishopAttacks(int square, U64 occupancy);
U64 getKnightAttacks(int square);
U64 getQueenAttacks(int square, U64 occupancy);
U64 getKingAttacks(int square);
int getPieceColour(int p, board * board);
int getPieceType(int p, board * board);

bool running = true;

int main()
{
    std::cout<<"Tranc Engine"<<std::endl;

    // define U64 variables and print them as boards


    board testBoard;
    
    for (int i = 0; i < 6; i++)
    {
        testBoard.white[i] = 0ULL;
        testBoard.black[i] = 0ULL;
    }
    testBoard.whiteOccupancy = 0ULL;
    testBoard.blackOccupancy = 0ULL;
    testBoard.occupancy = 0ULL;
    testBoard.whiteToMove = true;

    testBoard.white[pawn] = RANK_2;
    set_bit(testBoard.white[bishop], C1);
    set_bit(testBoard.white[bishop], F1);
    set_bit(testBoard.white[knight], B1);
    set_bit(testBoard.white[knight], G1);
    set_bit(testBoard.white[rook], A1);
    set_bit(testBoard.white[rook], H1);
    set_bit(testBoard.white[queen], D1);
    set_bit(testBoard.white[king], E1);

    testBoard.black[pawn] = RANK_7;
    set_bit(testBoard.black[pawn], B3);

    set_bit(testBoard.black[bishop], C8);
    set_bit(testBoard.black[bishop], F8);
    set_bit(testBoard.black[knight], B8);
    set_bit(testBoard.black[knight], G8);
    set_bit(testBoard.black[rook], A8);
    set_bit(testBoard.black[rook], H8);
    set_bit(testBoard.black[queen], D8);
    set_bit(testBoard.black[king], E8);

    for (int i = 0; i < 6; i++)
    {
        testBoard.whiteOccupancy |= testBoard.white[i];
    }

    for (int i = 0; i < 6; i++)
    {
        testBoard.blackOccupancy |= testBoard.black[i];
    }

    testBoard.occupancy = testBoard.whiteOccupancy | testBoard.blackOccupancy;

    std::string move;
    while (running)
    {
        printBoard(&testBoard);

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

        // check which piece is being moved from move start
        //      int getPieceType(board)

        int colour = getPieceColour(moveStart, &testBoard);

        if (colour != testBoard.whiteToMove)
        {
            std::cout<<"invalid move, wrong colour"<<std::endl;
            continue;
        }

        int type = getPieceType(moveStart, &testBoard);

        // calculate the attacks of the selected piece

        U64 attacks = 0ULL;

        switch (type)
        {
            case 0:
                attacks = getPawnAttacks(moveStart, testBoard.occupancy, testBoard.whiteToMove);
                break;
            case 1:
                attacks = getKnightAttacks(moveStart);
                break;
            case 2:
                attacks = getBishopAttacks(moveStart, testBoard.occupancy);
                break;
            case 3:
                attacks = getRookAttacks(moveStart, testBoard.occupancy);
                break;
            case 4:
                attacks = getQueenAttacks(moveStart, testBoard.occupancy);
                break;
            case 5:
                attacks = getKingAttacks(moveStart);
                break;
            default:
                printf("Error\n");
                continue;
        }

        printf("Attacks: \n");
        printBitBoard(attacks);

        // if the move end is not in the attacks then invalid
        if (!get_bit(attacks, moveEnd))
        {
            printf("invalid move, not in possible attack squares");
            continue;
        }

        // if the move is in attacks but the player already has a piece in that square then invalid
        if (get_bit(attacks, moveEnd) && 
            get_bit(testBoard.whiteToMove ? testBoard.whiteOccupancy : testBoard. blackOccupancy, moveEnd))
        {
            printf("invalid move, you cannot capture your own pieces");
            continue;
        }

        // if the player is checked and the move does not resolve the check then invalid
        //      TODO: Calculate checks

        // if this piece is pinned and the move does not resolve the pin then invalid
        //      TODO: Calculate pins
        
        // if the end square is in the attacks and the square is empty then move to it

        if (get_bit(attacks, moveEnd) && !get_bit(testBoard.occupancy, moveEnd))
        {
            printf("move piece\n");
        }


        // if the end square is in the attacks and the square is occupied by an opponent's peice then capture

    }

    return 0;
}

void printBoard(board * board)
{
    if (board->whiteToMove) printf("\nWhite to move\n");
    else                    printf("\nBlack to move\n");
    
    for (int i = 0; i < 64; i++)
    {
        // print ranks
        if (i%8 == 0) printf("%d  ", i/8 + 1);

        int type = getPieceType(i, board);
        int colour = getPieceColour(i, board);

        if (type == -1 || colour == -1)
        {
            std::cout<<".  ";
            if ((i+1)%8 == 0) std::cout<<std::endl;
            continue;
        }

        if (colour == 1) std::cout<<"w";
        if (colour == 0) std::cout<<"b";
        std::cout<<(Piece)type<<" ";

        if ((i+1)%8 == 0) std::cout<<std::endl;

    }
    printf("   A  B  C  D  E  F  G  H\n");
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

void movePiece(int start, int end, board * board)
{

    // will this function handle captures? not sure.




    // TODO: calculate if move generates a check
    // TODO: calculate if move pins a piece

}


void movePiece(board * board, int start, int end)
{
    if (get_bit(board->white[pawn], start))
    {
        clear_bit(board->white[pawn], start);
        set_bit(board->white[pawn], end);

        printf("moved white pawn\n");
    }
    else if (get_bit(board->black[pawn], start))
    {
        clear_bit(board->black[pawn], start);
        set_bit(board->black[pawn], end);
        printf("moved black pawn\n");
    }

    board->occupancy = board->white[pawn] | board->black[pawn];

}

U64 getPawnAttacks(int square, U64 occupancy, bool isWhite)
{
    U64 pawnMoves = 0ULL;

    int direction = isWhite ? 1 : -1;

    // if the pawn is in the final rank it has no moves
    // this is just a redundancy in case promotion doesn't work
    if ( isWhite && get_bit(RANK_8, square)) return pawnMoves;
    if (!isWhite && get_bit(RANK_1, square)) return pawnMoves;

    if (!(!isWhite && get_bit(FILE_A, square)) &&
        !( isWhite && get_bit(FILE_H, square)) &&
        get_bit(occupancy, square + (direction * 9)))
    {
        set_bit(pawnMoves, square + (direction * 9));
    }

    if (!( isWhite && get_bit(FILE_A, square)) &&
        !(!isWhite && get_bit(FILE_H, square)) &&
        get_bit(occupancy, square + (direction * 7)))
    {
        set_bit(pawnMoves, square + (direction * 7));
    }

    if (!get_bit(occupancy, square + (direction * 8)))
    {
        set_bit(pawnMoves, square + (direction * 8));

        // only check for square 2 ahead if the square 1 ahead is free
        if (get_bit(RANK_2, square) && !get_bit(occupancy, square + (direction * 16)))
        {
            set_bit(pawnMoves, square + (direction * 16));
        }
    }

    return pawnMoves;
}

U64 getRookAttacks(int square, U64 occupancy)
{
    U64 attacks = 0ULL;
    
    int rank = square / 8;
    int file = square % 8;
    
    for (int r = rank + 1; r < 8; r++)
    {
        set_bit(attacks, r * 8 + file);
        if (get_bit(occupancy, r * 8 + file)) break;
    }

    for (int r = rank - 1; r >= 0; r--)
    {
        set_bit(attacks, r * 8 + file);
        if (get_bit(occupancy, r * 8 + file)) break;
    }

    for (int f = file + 1; f < 8; f++)
    {
        set_bit(attacks, rank * 8 + f);
        if (get_bit(occupancy, rank * 8 + f)) break;
    }
    
    for (int f = file - 1; f >= 0; f--)
    {
        set_bit(attacks, rank * 8 + f);
        if (get_bit(occupancy, rank * 8 + f)) break;
    }
    
    return attacks;
}

U64 getBishopAttacks(int square, U64 occupancy)
{
    U64 attacks = 0ULL;
    
    int rank = square / 8;
    int file = square % 8;
    
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++)
    {
        set_bit(attacks, r * 8 + f);
        if (get_bit(occupancy, r * 8 + f)) break;
    }
    
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--)
    {
        set_bit(attacks, r * 8 + f);
        if (get_bit(occupancy, r * 8 + f)) break;
    }
    
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++)
    {
        set_bit(attacks, r * 8 + f);
        if (get_bit(occupancy, r * 8 + f)) break;
    }
    
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--)
    {
        set_bit(attacks, r * 8 + f);
        if (get_bit(occupancy, r * 8 + f)) break;
    }
    
    return attacks;
}

U64 getKnightAttacks(int square)
{

    U64 attacks = 0ULL;

    // north north west
    // -    Can't be in file A
    // -    Can't be in rank 1 or rank 2
    if (!get_bit(FILE_A | RANK_1 | RANK_2, square))
    {
        set_bit(attacks, square - ((2*8) + 1));
    }

    // north west west
    // -    Can't be in file A or file B
    // -    Can't be in rank 1
    if (!get_bit(FILE_A | FILE_B | RANK_1, square))
    {
        set_bit(attacks, square - ((1*8) + 2));
    }

    // south west west
    // -    Can't be in file A or file B
    // -    Can't be in rank 8
    if (!get_bit(FILE_A | FILE_B | RANK_8, square))
    {
        set_bit(attacks, square + ((1*8) - 2));
    }

    // south south west
    // -    Can't be in file A
    // -    Can't be in rank 7 or rank 8
    if (!get_bit(FILE_A | RANK_7 | RANK_8, square))
    {
        set_bit(attacks, square + ((2*8) - 1));
    }
    // south south east
    // -    Can't be file H
    // -    Can't be in rank 7 or rank 8
    if (!get_bit(FILE_H | RANK_7 | RANK_8, square))
    {
        set_bit(attacks, square + ((2*8) + 1));
    }

    // south east east
    // -    Can't be in file G or file H
    // -    Can't be in rank 8

    if (!get_bit(FILE_G | FILE_H | RANK_8, square))
    {
        set_bit(attacks, square + ((1*8) + 2));
    }

    // north east east
    // -    Can't be in file G or file H
    // -    Can't be in rank 1
    if (!get_bit(FILE_G | FILE_H | RANK_1, square))
    {
        set_bit(attacks, square - ((1*8) - 2));
    }
    // north north east
    // -    Can't be in file H
    // -    Can't be in rank 1 or rank 2
    if (!get_bit(FILE_H | RANK_1 | RANK_2, square))
    {
        set_bit(attacks, square - ((2*8) - 1));
    }

    return attacks;
}

U64 getKingAttacks(int square)
{

    U64 attacks = 0ULL;

    // north
    // -    Can't be in rank 1
    if (!get_bit(RANK_1, square))
    {
        set_bit(attacks, square - 8);
    }

    // north west
    // -    Can't be in file A
    // -    Can't be in rank 1
    if (!get_bit(FILE_A | RANK_1, square))
    {
        set_bit(attacks, square - (8 + 1));
    }

    // west
    // -    Can't be in file A
    if (!get_bit(FILE_A, square))
    {
        set_bit(attacks, square - 1);
    }

    // south west
    // -    Can't be in file A
    // -    Can't be in rank 8
    if (!get_bit(FILE_A | RANK_8, square))
    {
        set_bit(attacks, square + (8 - 1));
    }
    // south
    // -    Can't be in rank 8
    if (!get_bit(RANK_8, square))
    {
        set_bit(attacks, square + 8);
    }

    // south east
    // -    Can't be in file H
    // -    Can't be in rank 8

    if (!get_bit(FILE_H | RANK_8, square))
    {
        set_bit(attacks, square + (8 + 1));
    }

    // east
    // -    Can't be in file H
    if (!get_bit(FILE_H, square))
    {
        set_bit(attacks, square + 1);
    }
    // north east
    // -    Can't be in file H
    // -    Can't be in rank 1
    if (!get_bit(FILE_H | RANK_1, square))
    {
        set_bit(attacks, square - (8 - 1));
    }

    // TODO: Castle?

    return attacks;
}

U64 getQueenAttacks(int square, U64 occupancy)
{
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}

int getPieceColour(int p, board * board)
{
    if (get_bit(board->whiteOccupancy, p))
    {
        return 1;
    }
    else if (get_bit(board->blackOccupancy, p))
    {
        return 0;
    }

    return -1;
}

int getPieceType(int p, board * board)
{
    for (int i = 0; i < 6; i++)
    {
        if (get_bit(board->white[i], p))
        {
            return i;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        if (get_bit(board->black[i], p))
        {
            return i;
        }
    }

    return -1;
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