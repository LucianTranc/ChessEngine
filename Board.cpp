#include "Board.hpp"

void Board::printBoard()
{
    if (turn == white)   printf("\nWhite to move\n");
    else                        printf("\nBlack to move\n");
    
    for (int i = 0; i < 64; i++)
    {
        // print ranks
        if (i%8 == 0) printf("%d  ", i/8 + 1);

        int type = getPieceType(i);
        int colour = getPieceColour(i);

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

void Board::deletePiece(int position)
{
    int type = getPieceType(position);
    int colour = getPieceColour(position);

    // relative compliment of occupancy
    // basically subtract the given pieces from the list of w/b pieces
    occupancy[colour] &= ~pieces[colour][type];

    // flip the bits of the start and end positions
    clear_bit(pieces[colour][type], position);

    // add the given piece back to the set
    occupancy[colour] |= pieces[colour][type];

    totalOccupancy = occupancy[white] | occupancy[black];

}

void Board::movePiece(int start, int end)
{

    // check if capture
    bool capture = get_bit(totalOccupancy, end);
    int type = getPieceType(start);
    int colour = getPieceColour(start);

    // move
    if (!capture)
    {
        // relative compliment of occupancy
        // basically subtract the given pieces from the list of w/b pieces
        occupancy[colour] &= ~pieces[colour][type];

        // flip the bits of the start and end positions
        clear_bit(pieces[colour][type], start);
        set_bit(pieces[colour][type], end);

        // add the given piece back to the set
        occupancy[colour] |= pieces[colour][type];

        totalOccupancy = occupancy[white] | occupancy[black];
    }
    else
    {
        int capturedType = getPieceType(end);
        int capturedColour = colour ? white : black;

        occupancy[colour] &= ~pieces[colour][type];
        occupancy[capturedColour] &= ~pieces[capturedColour][capturedType];

        clear_bit(pieces[colour][type], start);
        set_bit(pieces[colour][type], end);

        clear_bit(pieces[capturedColour][capturedType], end);

        occupancy[colour] |= pieces[colour][type];
        occupancy[capturedColour] |= pieces[capturedColour][capturedType];

        totalOccupancy = occupancy[white] | occupancy[black];

    }
    printf("Calculating pins:\n");

    // TODO: calculate if move pins a piece

    // for every piece on the board, if deleting it means that the side is in check,
    // then the piece is pinned

    pinnedPieces = 0ULL;

    U64 whitePieces = occupancy[white];
    U64 blackPieces = occupancy[black];

    Board tempBoard;

    printf("WHITE\n");

    while (whitePieces)
    {
        tempBoard = *this;

        int piece = pop_LSB(whitePieces);

        std::cout<<"W: peice: "<<(Square)piece<<std::endl;

        int checksWithPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[white][king]), black));

        //printf("checks with piece = %d\n", checksWithPiece);

        tempBoard.deletePiece(piece);

        int checksWithoutPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[white][king]), black));

        //printf("checks without piece = %d\n", checksWithoutPiece);

        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(pinnedPieces, piece);
        }
    }

    printf("BLACK\n");

    while (blackPieces)
    {
        tempBoard = *this;

        int piece = pop_LSB(blackPieces);

        std::cout<<"B: peice: "<<(Square)piece<<std::endl;

        int checksWithPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[black][king]), white));

        //printf("checks with piece = %d\n", checksWithPiece);

        tempBoard.deletePiece(piece);

        int checksWithoutPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[black][king]), white));

        //printf("checks without piece = %d\n", checksWithoutPiece);

        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(pinnedPieces, piece);
        }

    }

    /*bool whiteHasLegalMove = false;
    bool blackHasLegalMove = false;

    whitePieces = occupancy[white];
    blackPieces = occupancy[black];

    while (whitePieces)
    {
        int piece = pop_LSB(whitePieces);

        if (getLegalMoves(piece))
        {
            whiteHasLegalMove = true;
            break;
        }
    }

    while (blackPieces)
    {
        int piece = pop_LSB(blackPieces);

        if (getLegalMoves(piece))
        {
            blackHasLegalMove = true;
            break;
        }
    }

    if (!whiteHasLegalMove)
    {
        if (getAttackers(get_LSB(tempBoard.pieces[white][king]), black))
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
        if (getAttackers(get_LSB(tempBoard.pieces[black][king]), white))
        {
            printf("Checkmate, Black wins\n");
        }
        else
        {
            printf("Stalemate\n");
        }
    }*/

}

int Board::getPieceColour(int p)
{
    if (get_bit(occupancy[white], p))
    {
        return white;
    }
    else if (get_bit(occupancy[black], p))
    {
        return black;
    }

    return -1;
}

int Board::getPieceType(int p)
{
    for (int i = 0; i < 6; i++)
    {
        if (get_bit(pieces[white][i], p) || get_bit(pieces[black][i], p))
        {
            return i;
        }
    }

    return -1;
}

U64 Board::getLegalMoves(int p)
{
    U64 legalMoves = 0ULL;

    int colour = getPieceColour(p);

    // if it's not that piece's turn then there are no legal moves
    if (colour != turn)
    {
        return legalMoves;
    }

    int type = getPieceType(p);

    // calculate the attacks of the selected piece
    switch (type)
    {
        case 0:
            legalMoves = getPawnMoves(p, totalOccupancy, turn);
            break;
        case 1:
            legalMoves = getKnightAttacks(p);
            break;
        case 2:
            legalMoves = getBishopAttacks(p, totalOccupancy);
            break;
        case 3:
            legalMoves = getRookAttacks(p, totalOccupancy);
            break;
        case 4:
            legalMoves = getQueenAttacks(p, totalOccupancy);
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
    legalMoves &= ~occupancy[colour];

    printf("Psuedo Legal Moves: \n");
    printBitBoard(legalMoves);

    printf("Pinned pieces: \n");
    printBitBoard(pinnedPieces);

    // if the player is checked and the move does not resolve the check then invalid

    // if the piece is pinned or there are pieces attacking the king
    if (get_bit(pinnedPieces, p) || getAttackers(get_LSB(pieces[colour][king]), colour ? Colour::white : Colour::black))
    {
        printf("The king is in check or piece is pinned\n");

        // initialize a copy of the current legal moves
        U64 tempLegalMoves = legalMoves;

        // create a new board !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!
        Board tempBoard;

        // while there are moves to check
        while (tempLegalMoves)
        {
            // set the new board to the current value of the board
            tempBoard = *this;

            // pop the current legal move
            int tempMove = pop_LSB(tempLegalMoves);

            std::cout<<"checking move "<<(Square)tempMove<<std::endl;

            // move the piece to the current move on the copy of the board
            tempBoard.movePiece(p, tempMove);

            printf("attackers: \n");
            printBitBoard(tempBoard.getAttackers(get_LSB(tempBoard.pieces[colour][king]), colour ? white : black));

            // if the king has attackers after making that move then remove it from the legal moves
            if (tempBoard.getAttackers(get_LSB(tempBoard.pieces[colour][king]), colour ? white : black))
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
U64 Board::getAttackers(int target, int attackingColour)
{
    U64 attackers = 0ULL;
    U64 possibleAttackers = occupancy[attackingColour];

    while (possibleAttackers)
    {
        int attackerPosition = pop_LSB(possibleAttackers);
        int type = getPieceType(attackerPosition);

        U64 attacks = 0ULL;

        switch (type)
        {
            case 0:
                attacks = getPossiblePawnAttacks(attackerPosition, totalOccupancy, (Colour)attackingColour);
                break;
            case 1:
                attacks = getKnightAttacks(attackerPosition);
                break;
            case 2:
                attacks = getBishopAttacks(attackerPosition, totalOccupancy);
                break;
            case 3:
                attacks = getRookAttacks(attackerPosition, totalOccupancy);
                break;
            case 4:
                attacks = getQueenAttacks(attackerPosition, totalOccupancy);
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