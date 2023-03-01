#include <bitset>
#include <string>
#include <string.h>
#include <cctype>
#include <ctype.h>
#include "Board.hpp"
#include "ChessAI.hpp"

#define PLY 4

// replace with printBoardUnicode() for prettier prints
#define PRINTFUNCTION printBoardUnicode()

bool running = true;

int main()
{
    Board testBoard;
    
    // initializing the board
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

    // setting all the pieces
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


    std::cout<<"\n\u2658 Lucian's Chess Engine \u2658"<<std::endl;

    bool startGame = false;
    std::string players;
    int numPlayers = 0;

    while (!startGame)
    {
        std::cout<<"\nEnter number of players (1 or 2):"<<std::endl;
        std::getline(std::cin, players);

        if (strlen(players.c_str()) != 1 || (players[0] != '1' && players[0] != '2'))
        {
            std::cout<<"Invalid input"<<std::endl;
        }
        else
        {
            numPlayers = players[0] - '0';
            startGame = true;
        }
    }

    // AI is controlling black
    ChessAI AI(black);

    std::string move;
    while (running)
    {
        testBoard.PRINTFUNCTION;

        int moveStart = 0;
        int moveEnd = 0;

        if (testBoard.turn == white || numPlayers == 2)
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

            moveStart = ((move[1] - '0' - 1) * 8) + (islower(move[0]) ? toupper(move[0]) : move[0]) - 'A';
            moveEnd = ((move[4] - '0' - 1) * 8) + (islower(move[3]) ? toupper(move[3]) : move[3]) - 'A';

            // valid move?
            // calculate all the legal moves of the piece selected
            
            U64 legalMoves = testBoard.getLegalMoves(moveStart);

            // if the move end is not in the legal moves then invalid
            if (!get_bit(legalMoves, moveEnd))
            {
                printf("invalid move, not a legal move");
                continue;
            }
        
        }
        else
        {
            // AI will look 4 moves ahead. 
            AI.getMove(&moveStart, &moveEnd, &testBoard, PLY);
            printf("\nevaluations: %d\n", AI.evaluations);
            std::cout<<"Move: "<<(Square)moveStart<<" "<<(Square)moveEnd<<std::endl;
        }

        // move piece
        testBoard.movePiece(moveStart, moveEnd);
        testBoard.turn = testBoard.turn ? Colour::white : Colour::black;

        GameState state = testBoard.getGameState();
        
        switch (state)
        {
            case active:
                continue;
            case whiteWin:
                testBoard.PRINTFUNCTION;
                printf("Checkmate, White wins\n");
                return 0;
            case blackWin:
                testBoard.PRINTFUNCTION;
                printf("Checkmate, Black wins\n");
                return 0;
            case stalemate:
                testBoard.PRINTFUNCTION;
                printf("Stalemate\n");
                return 0;
            
        }

    }

    return 0;
}

