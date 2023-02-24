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


#define set_bit(b, i) ((b) |= (1ULL << (i)))
#define get_bit(b, i) ((b) & (1ULL << (i)))  
#define clear_bit(b, i) ((b) &= ~(1ULL << (i))) 
#define get_LSB(b) (__builtin_ctzll(b))

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

enum Piece{
    pawn, knight, bishop, rook, queen, king
};

// overloading << operator to print Piece enum names
std::ostream &operator << (std::ostream& stream, Piece p)
{
    const std::string squareNames[] = {
        "P", "N", "B", "R", "Q", "K"
    };
   return stream << squareNames[p];
}

enum Colour {
    white, black
};

// overloading << operator to print Colour enum names
std::ostream &operator << (std::ostream& stream, Colour c)
{
    const std::string colourNames[] = {
        "w", "b"
    };
   return stream << colourNames[c];
}

struct board
{
    U64 pieces[2][6];
    U64 occupancy[2];
    U64 totalOccupancy;
    U64 pinnedPieces;
    Colour turn;
} 
typedef board;

void printBoard(board * board);
void printBitBoard(U64 bitboard);
void movePiece(int start, int end, board * board);
void deletePiece(int position, board * board);
U64 getPawnMoves(int square, U64 occupancy, Colour turn);
U64 getLegalPawnAttacks(int square, U64 occupancy, Colour turn);
U64 getPossiblePawnAttacks(int square, U64 occupancy, Colour turn);
U64 getRookAttacks(int square, U64 occupancy);
U64 getBishopAttacks(int square, U64 occupancy);
U64 getKnightAttacks(int square);
U64 getQueenAttacks(int square, U64 occupancy);
U64 getKingAttacks(int square);
int getPieceColour(int p, board * board);
int getPieceType(int p, board * board);
U64 getLegalMoves(int p, board * board);
U64 getAttackers(int target, int attackingColour, board * board);
int countSetBits(U64 board);

bool running = true;

int main()
{
    std::cout<<"Tranc Engine"<<std::endl;

    board testBoard;
    
    for (int i = 0; i < 6; i++)
    {
        testBoard.pieces[0][i] = 0ULL;
        testBoard.pieces[1][i] = 0ULL;
    }
    testBoard.occupancy[white] = 0ULL;
    testBoard.occupancy[black] = 0ULL;
    testBoard.totalOccupancy = 0ULL;
    testBoard.pinnedPieces = 0ULL;
    testBoard.turn = white;

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
        // calculate all the legal moves of the piece selected
        
        U64 legalMoves = getLegalMoves(moveStart, &testBoard);

        // if the move end is not in the legal moves then invalid
        if (!get_bit(legalMoves, moveEnd))
        {
            printf("invalid move, not a legal move");
            continue;
        }
        
        // move piece
        printf("move piece\n");
        movePiece(moveStart, moveEnd, &testBoard);
        testBoard.turn = testBoard.turn ? white : black;



    }

    return 0;
}

U64 getLegalMoves(int p, board * board)
{
    U64 legalMoves = 0ULL;

    int colour = getPieceColour(p, board);

    // if it's not that piece's turn then there are no legal moves
    if (colour != board->turn)
    {
        return legalMoves;
    }

    int type = getPieceType(p, board);

    // calculate the attacks of the selected piece
    switch (type)
    {
        case 0:
            legalMoves = getPawnMoves(p, board->totalOccupancy, board->turn);
            break;
        case 1:
            legalMoves = getKnightAttacks(p);
            break;
        case 2:
            legalMoves = getBishopAttacks(p, board->totalOccupancy);
            break;
        case 3:
            legalMoves = getRookAttacks(p, board->totalOccupancy);
            break;
        case 4:
            legalMoves = getQueenAttacks(p, board->totalOccupancy);
            break;
        case 5:
            legalMoves = getKingAttacks(p);
            break;
        default:
            legalMoves = 0ULL;
    }

    printf("Attacks: \n");
    printBitBoard(legalMoves);

    // subtract the occupancy of team pieces
    legalMoves &= ~board->occupancy[colour];

    printf("Psuedo Legal Moves: \n");
    printBitBoard(legalMoves);

    printf("Pinned pieces: \n");
    printBitBoard(board->pinnedPieces);

    // if the player is checked and the move does not resolve the check then invalid

    // if the piece is pinned or there are pieces attacking the king
    if (get_bit(board->pinnedPieces, p) || getAttackers(get_LSB(board->pieces[colour][king]), colour ? white : black, board))
    {
        printf("The king is in check or piece is pinned\n");

        // initialize a copy of the current legal moves
        U64 tempLegalMoves = legalMoves;

        // create a new board
        struct board tempBoard;

        // while there are moves to check
        while (tempLegalMoves)
        {
            // set the new board to the current value of the board
            tempBoard = *board;

            // pop the current legal move
            int tempMove = pop_LSB(tempLegalMoves);

            // move the piece to the current move on the copy of the board
            movePiece(p, tempMove, &tempBoard);

            // if the king has attackers after making that move then remove it from the legal moves
            if (getAttackers(get_LSB(tempBoard.pieces[colour][king]), colour ? white : black, &tempBoard))
            {
                clear_bit(legalMoves, tempMove);
            }

        }

    }
    
    printf("Legal Moves: \n");
    printBitBoard(legalMoves);


    return legalMoves;
}

// get all pieces that attack position "target" that have a colour "colour", 
U64 getAttackers(int target, int attackingColour, board * board)
{
    U64 attackers = 0ULL;
    U64 possibleAttackers = board->occupancy[attackingColour];

    while (possibleAttackers)
    {
        int attackerPosition = pop_LSB(possibleAttackers);
        int type = getPieceType(attackerPosition, board);

        U64 attacks = 0ULL;

        switch (type)
        {
            case 0:
                attacks = getPossiblePawnAttacks(attackerPosition, board->totalOccupancy, (Colour)attackingColour);
                break;
            case 1:
                attacks = getKnightAttacks(attackerPosition);
                break;
            case 2:
                attacks = getBishopAttacks(attackerPosition, board->totalOccupancy);
                break;
            case 3:
                attacks = getRookAttacks(attackerPosition, board->totalOccupancy);
                break;
            case 4:
                attacks = getQueenAttacks(attackerPosition, board->totalOccupancy);
                break;
            case 5:
                attacks = getKingAttacks(attackerPosition);
                break;
            default:
                attacks = 0ULL;
        }

        if (get_bit(attacks, target))
        {
            set_bit(attackers, attackerPosition);
        }
    }
    return attackers;
}

void printBoard(board * board)
{
    if (board->turn == white)   printf("\nWhite to move\n");
    else                        printf("\nBlack to move\n");
    
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

        std::cout<<(Colour)colour;
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

void deletePiece(int position, board * board)
{
    int type = getPieceType(position, board);
    int colour = getPieceColour(position, board);

    // relative compliment of occupancy
    // basically subtract the given pieces from the list of w/b pieces
    board->occupancy[colour] &= ~board->pieces[colour][type];

    // flip the bits of the start and end positions
    clear_bit(board->pieces[colour][type], position);

    // add the given piece back to the set
    board->occupancy[colour] |= board->pieces[colour][type];

    board->totalOccupancy = board->occupancy[white] | board->occupancy[black];

}

void movePiece(int start, int end, board * board)
{

    // check if capture
    bool capture = get_bit(board->totalOccupancy, end);
    int type = getPieceType(start, board);
    int colour = getPieceColour(start, board);

    // move
    if (!capture)
    {
        // relative compliment of occupancy
        // basically subtract the given pieces from the list of w/b pieces
        board->occupancy[colour] &= ~board->pieces[colour][type];

        // flip the bits of the start and end positions
        clear_bit(board->pieces[colour][type], start);
        set_bit(board->pieces[colour][type], end);

        // add the given piece back to the set
        board->occupancy[colour] |= board->pieces[colour][type];

        board->totalOccupancy = board->occupancy[white] | board->occupancy[black];
    }
    else
    {
        int capturedType = getPieceType(end, board);
        int capturedColour = colour ? white : black;

        board->occupancy[colour] &= ~board->pieces[colour][type];
        board->occupancy[capturedColour] &= ~board->pieces[capturedColour][capturedType];

        clear_bit(board->pieces[colour][type], start);
        set_bit(board->pieces[colour][type], end);

        clear_bit(board->pieces[capturedColour][capturedType], end);

        board->occupancy[colour] |= board->pieces[colour][type];
        board->occupancy[capturedColour] |= board->pieces[capturedColour][capturedType];

        board->totalOccupancy = board->occupancy[white] | board->occupancy[black];

    }
    printf("Calculating pins:\n");

    // TODO: calculate if move pins a piece

    // for every piece on the board, if deleting it means that the side is in check,
    // then the piece is pinned

    board->pinnedPieces = 0ULL;

    U64 whitePieces = board->occupancy[white];
    U64 blackPieces = board->occupancy[black];

    struct board tempBoard;

    printf("WHITE\n");

    while (whitePieces)
    {
        tempBoard = *board;

        int piece = pop_LSB(whitePieces);

        std::cout<<"peice: "<<(Square)piece<<std::endl;

        int checksWithPiece = countSetBits(getAttackers(get_LSB(tempBoard.pieces[white][king]), black, &tempBoard));

        printf("checks with piece = %d\n", checksWithPiece);

        deletePiece(piece, &tempBoard);

        int checksWithoutPiece = countSetBits(getAttackers(get_LSB(tempBoard.pieces[white][king]), black, &tempBoard));

        printf("checks without piece = %d\n", checksWithoutPiece);

        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(board->pinnedPieces, piece);
        }
    }

    printf("BLACK\n");

    while (blackPieces)
    {
        tempBoard = *board;

        int piece = pop_LSB(blackPieces);

        std::cout<<"peice: "<<(Square)piece<<std::endl;

        int checksWithPiece = countSetBits(getAttackers(get_LSB(tempBoard.pieces[black][king]), white, &tempBoard));

        printf("checks with piece = %d\n", checksWithPiece);

        deletePiece(piece, &tempBoard);

        int checksWithoutPiece = countSetBits(getAttackers(get_LSB(tempBoard.pieces[black][king]), white, &tempBoard));

        printf("checks without piece = %d\n", checksWithoutPiece);

        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(board->pinnedPieces, piece);
        }

    }

    bool whiteHasLegalMove = false;
    bool blackHasLegalMove = false;

    whitePieces = board->occupancy[white];
    blackPieces = board->occupancy[black];

    while (whitePieces)
    {
        int piece = pop_LSB(whitePieces);

        if (getLegalMoves(piece, board))
        {
            whiteHasLegalMove = true;
            break;
        }
    }

    while (blackPieces)
    {
        int piece = pop_LSB(blackPieces);

        if (getLegalMoves(piece, board))
        {
            blackHasLegalMove = true;
            break;
        }
    }

    if (!whiteHasLegalMove)
    {
        if (getAttackers(get_LSB(tempBoard.pieces[white][king]), black, board))
        {
            printf("Checkmate, Black wins\n");
        }
        else
        {
            printf("Stalemate\n");
        }
    }

    if (!blackHasLegalMove)
    {
        if (getAttackers(get_LSB(tempBoard.pieces[black][king]), white, board))
        {
            printf("Checkmate, Black wins\n");
        }
        else
        {
            printf("Stalemate\n");
        }
    }

}

int countSetBits(U64 board)
{
    unsigned int count = 0;
    while (board) {
        count += board & 1;
        board >>= 1;
    }
    return count;
}

U64 getPawnMoves(int square, U64 occupancy, Colour turn)
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

U64 getLegalPawnAttacks(int square, U64 occupancy, Colour turn)
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

U64 getPossiblePawnAttacks(int square, U64 occupancy, Colour turn)
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
    if (get_bit(board->occupancy[white], p))
    {
        return white;
    }
    else if (get_bit(board->occupancy[black], p))
    {
        return black;
    }

    return -1;
}

int getPieceType(int p, board * board)
{
    for (int i = 0; i < 6; i++)
    {
        if (get_bit(board->pieces[white][i], p) || get_bit(board->pieces[black][i], p))
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