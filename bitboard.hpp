// Lucian Tranc
// Chess Engine
// This file contains constants, typedefs, structs and functions necessary for chess bitboards

#include <iostream>

// unsigned 64 bit integer datatype
// allows each of the 64 bits to represent 1 square on chess board
typedef u_int64_t U64;

// bitboards representing each of the files and ranks
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

// macros bit manipulation taken from:
// https://aticleworld.com/macros-for-bit-manipulation-c-cpp/
#define set_bit(b, i) ((b) |= (1ULL << (i)))
#define get_bit(b, i) ((b) & (1ULL << (i)))  
#define clear_bit(b, i) ((b) &= ~(1ULL << (i))) 
#define get_LSB(b) (__builtin_ctzll(b))

// function that returns the index 
inline int pop_LSB(U64 &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}

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

// overloading << operator to print Square enum names (used mostly for debugging)
[[maybe_unused]] static std::ostream &operator << (std::ostream& stream, Square s)
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

// enum for the piece types
enum Piece{
    pawn, knight, bishop, rook, queen, king
};

// overloading << operator to print Piece enum names. (used in Board::printBoard)
[[maybe_unused]] static std::ostream &operator << (std::ostream& stream, Piece p)
{
    const std::string squareNames[] = {
        "P", "N", "B", "R", "Q", "K"
    };
   return stream << squareNames[p];
}

enum Colour {
    white, black
};

// overloading << operator to print Colour enum names (used in Board::printBoard)
[[maybe_unused]] inline std::ostream &operator << (std::ostream& stream, Colour c)
{
    const std::string colourNames[] = {
        "w", "b"
    };
   return stream << colourNames[c];
}

// enum for the game states
enum GameState {
    active, whiteWin, blackWin, stalemate
};

// helper function that prints a bitboard (used for debugging)
inline void printBitBoard(U64 bitboard)
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

// function that prints the number of bits set to 1 in a bitboard
inline int countSetBits(U64 board)
{
    unsigned int count = 0;
    while (board) {
        count += board & 1;
        board >>= 1;
    }
    return count;
}