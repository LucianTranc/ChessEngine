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

    if      (!a1Moved && start == A1) a1Moved = true;
    else if (!h1Moved && start == H1) h1Moved = true;
    else if (!a8Moved && start == A8) a8Moved = true;
    else if (!h8Moved && start == H8) h8Moved = true;
    else if (!whiteKingMoved && start == E1) whiteKingMoved = true;
    else if (!blackKingMoved && start == E8) blackKingMoved = true;
    
    // check if castle

    if (type == king && (abs(start - end) == 2))
    {
        printf("White King-side castle\n");

        int rookStart = 0;
        int rookEnd = 0;

        if (start == E1 && end == G1)
        {
            rookStart = H1;
            rookEnd = F1;
        } 
        else if (start == E1 && end == C1)
        {
            rookStart = A1;
            rookEnd = D1;
        }
        else if (start == E8 && end == G8)
        {
            rookStart = H8;
            rookEnd = F8;
        }
        else if (start == E8 && end == C8)
        {
            rookStart = A8;
            rookEnd = D8;
        }

        occupancy[colour] &= ~pieces[colour][rook];
        clear_bit(pieces[colour][rook], rookStart);
        set_bit(pieces[colour][rook], rookEnd);

        occupancy[colour] |= pieces[colour][rook];

        totalOccupancy = occupancy[white] | occupancy[black];
    }

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

    Board tempBoard;

    U64 possibleWhitePins = getBishopAttacks(get_LSB(pieces[white][king]), totalOccupancy);
    possibleWhitePins |= getRookAttacks(get_LSB(pieces[white][king]), totalOccupancy);
    U64 whitePieces = occupancy[white] & possibleWhitePins;

    while (whitePieces)
    {
        tempBoard = *this;
        int piece = pop_LSB(whitePieces);
        int checksWithPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[white][king]), black));
        tempBoard.deletePiece(piece);
        int checksWithoutPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[white][king]), black));

        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(pinnedPieces, piece);
        }
    }

    U64 possibleBlackPins = getBishopAttacks(get_LSB(pieces[black][king]), totalOccupancy);
    possibleBlackPins |= getRookAttacks(get_LSB(pieces[black][king]), totalOccupancy);
    U64 blackPieces = occupancy[black] & possibleBlackPins;

    while (blackPieces)
    {
        tempBoard = *this;
        int piece = pop_LSB(blackPieces);
        int checksWithPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[black][king]), white));
        tempBoard.deletePiece(piece);
        int checksWithoutPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[black][king]), white));

        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(pinnedPieces, piece);
        }

    }

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
                // check if the king is in check
                // check if the spaces between rook and king are clear.
                // check if the square the king will pass over or the
                // square the king will land in, is under attack.
            if (colour == white && !whiteKingMoved && !a1Moved
                && !getAttackers(get_LSB(pieces[colour][king]), black)
                && !get_bit(totalOccupancy, B1) && !get_bit(totalOccupancy, C1) && !get_bit(totalOccupancy, D1)
                && !getAttackers(C1, black) && !getAttackers(D1, black))
            {
                printf("setting white queen side castle\n");
                set_bit(legalMoves, C1);
            }
            else if (colour == white && !whiteKingMoved && !h1Moved
                && !getAttackers(get_LSB(pieces[colour][king]), black)
                && !get_bit(totalOccupancy, F1) && !get_bit(totalOccupancy, G1)
                && !getAttackers(F1, black) && !getAttackers(G1, black))
            {
                printf("setting white king side castle\n");
                set_bit(legalMoves, G1);
            }
            else if (colour == black && !blackKingMoved && !a8Moved
                && !getAttackers(get_LSB(pieces[colour][king]), white)
                && !get_bit(totalOccupancy, B8) && !get_bit(totalOccupancy, C8) && !get_bit(totalOccupancy, D8)
                && !getAttackers(C8, white) && !getAttackers(D8, white))
            {
                printf("setting black queen side castle\n");
                set_bit(legalMoves, C8);
            }
            else if (colour == black && !blackKingMoved && !h8Moved
                && !getAttackers(get_LSB(pieces[colour][king]), white)
                && !get_bit(totalOccupancy, F8) && !get_bit(totalOccupancy, G8)
                && !getAttackers(F8, white) && !getAttackers(G8, white))
            {
                printf("setting black king side castle\n");
                set_bit(legalMoves, G8);
            }
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

            // move the piece to the current move on the copy of the board
            tempBoard.movePiece(p, tempMove);

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

GameState Board::getGameState()
{
    bool whiteHasLegalMove = false;
    bool blackHasLegalMove = false;

    U64 whitePieces = occupancy[white];
    U64 blackPieces = occupancy[black];

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

    if (whiteHasLegalMove) printf("white has legal move\n");
    if (blackHasLegalMove) printf("black has legal move\n");

    if (turn == white && !whiteHasLegalMove)
    {
        if (getAttackers(get_LSB(pieces[white][king]), black))
        {
            return blackWin;
        }
        else
        {
            return stalemate;
        }
    }

    if (turn == black && !blackHasLegalMove)
    {
        if (getAttackers(get_LSB(pieces[black][king]), white))
        {
            return whiteWin;
        }
        else
        {
            return stalemate;
        }
    }

    return active;
}