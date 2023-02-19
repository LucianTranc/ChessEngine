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
    U64 whitePawns;
    U64 blackPawns;
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
void movePiece(board * board, position start, position end);

bool running = true;

int main()
{
    std::cout<<"Tranc Engine"<<std::endl;

    // define U64 variables and print them as boards


    printf("\n\n");

    board testBoard = {RANK_2, RANK_7};

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

        // move peice

        movePiece(&testBoard, moveStart, moveEnd);

        printBoard(testBoard);

    }


    return 0;
}

void printBoard(board board)
{
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
        if ((bitboard >> (i) & 1) == 1)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }

        if ((i+1)%8 == 0) printf("\n");
    }
    printf("\n");
    return;
}

int getPositionIndex(position pos)
{
    return pos.file - 'A' + ((pos.rank - 1) * 8);
}

void movePiece(board * board, position start, position end)
{
    if (get_bit(board->whitePawns, getPositionIndex(start)))
    {
        clear_bit(board->whitePawns, getPositionIndex(start));
        set_bit(board->whitePawns, getPositionIndex(end));
        printf("moved white pawn\n");
    }
    else if (get_bit(board->blackPawns, getPositionIndex(start)))
    {
        clear_bit(board->blackPawns, getPositionIndex(start));
        set_bit(board->blackPawns, getPositionIndex(end));
        printf("moved black pawn\n");
    }
}