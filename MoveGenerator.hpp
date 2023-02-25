#pragma once
#include "bitboard.hpp"

static U64 getLegalPawnAttacks(int square, U64 occupancy, Colour turn)
{
    U64 pawnAttacks = 0ULL;

    int direction = turn == white ? 1 : -1;

    if ( (turn == white) && get_bit(RANK_8, square)) return pawnAttacks;
    if (!(turn == white) && get_bit(RANK_1, square)) return pawnAttacks;

    if (!(!(turn == white) && get_bit(FILE_A, square)) &&
        !( (turn == white) && get_bit(FILE_H, square)) &&
        get_bit(occupancy, square + (direction * 9)))
    {
        set_bit(pawnAttacks, square + (direction * 9));
    }

    if (!( (turn == white) && get_bit(FILE_A, square)) &&
        !(!(turn == white) && get_bit(FILE_H, square)) &&
        get_bit(occupancy, square + (direction * 7)))
    {
        set_bit(pawnAttacks, square + (direction * 7));
    }

    return pawnAttacks;

}

static U64 getPawnMoves(int square, U64 occupancy, Colour turn)
{
    U64 pawnMoves = 0ULL;

    int direction = turn == white ? 1 : -1;

    // if the pawn is in the final rank it has no moves
    // this is just a redundancy in case promotion doesn't work
    if ( (turn == white) && get_bit(RANK_8, square)) return pawnMoves;
    if (!(turn == white) && get_bit(RANK_1, square)) return pawnMoves;

    if (!get_bit(occupancy, square + (direction * 8)))
    {
        set_bit(pawnMoves, square + (direction * 8));

        // only check for square 2 ahead if the square 1 ahead is free
        if ((((turn == white) && get_bit(RANK_2, square)) ||
             ((turn == black) && get_bit(RANK_7, square))) &&
            !get_bit(occupancy, square + (direction * 16)))
        {
            set_bit(pawnMoves, square + (direction * 16));
        }
    }

    return pawnMoves | getLegalPawnAttacks(square, occupancy, turn);
}

static U64 getPossiblePawnAttacks(int square, U64 occupancy, Colour turn)
{
    U64 pawnAttacks = 0ULL;

    int direction = turn == white ? 1 : -1;

    if ( (turn == white) && get_bit(RANK_8, square)) return pawnAttacks;
    if (!(turn == white) && get_bit(RANK_1, square)) return pawnAttacks;

    if (!(!(turn == white) && get_bit(FILE_A, square)) &&
        !( (turn == white) && get_bit(FILE_H, square)))
    {
        set_bit(pawnAttacks, square + (direction * 9));
    }

    if (!( (turn == white) && get_bit(FILE_A, square)) &&
        !(!(turn == white) && get_bit(FILE_H, square)))
    {
        set_bit(pawnAttacks, square + (direction * 7));
    }

    return pawnAttacks;

}


static U64 getRookAttacks(int square, U64 occupancy)
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

static U64 getBishopAttacks(int square, U64 occupancy)
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

static U64 getKnightAttacks(int square)
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

static U64 getKingAttacks(int square)
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

static U64 getQueenAttacks(int square, U64 occupancy)
{
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}
