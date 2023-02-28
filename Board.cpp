#include "Board.hpp"

// print the board
void Board::printBoard()
{
    if (turn == white)   printf("\nWhite to move\n");
    else                 printf("\nBlack to move\n");
    
    for (int i = 0; i < 64; i++)
    {
        // print ranks
        if (i%8 == 0) printf("%d  ", i/8 + 1);

        // get square information
        int type = getPieceType(i);
        int colour = getPieceColour(i);

        // if the square is empty
        if (type == -1 || colour == -1)
        {
            std::cout<<".  ";
            if ((i+1)%8 == 0) std::cout<<std::endl;
            continue;
        }

        // print the square
        std::cout<<(Colour)colour;
        std::cout<<(Piece)type<<" ";

        // newline every 8 squares
        if ((i+1)%8 == 0) std::cout<<std::endl;

    }
    printf("   A  B  C  D  E  F  G  H\n");
    return;
}

// prints the board with unicode chess piece characters and with white on bottom
void Board::printBoardUnicode()
{
    if (turn == white)   printf("\nWhite to move\n");
    else                 printf("\nBlack to move\n");
    
    for (int i = 7; i >= 0; i--)
    {
        printf("%d  ", i + 1);

        for (int j = 0; j < 8; j++)
        {
            int p = (i*8) + j;
            // print ranks

            // get square information
            int type = getPieceType(p);
            int colour = getPieceColour(p);

            // if the square is empty
            if (type == -1 || colour == -1)
            {
                std::cout<<".  ";
                continue;
            }

            if (colour == white)
            {
                switch (type)
                {
                    case pawn:
                        std::cout<<"\u265F"<<"  ";
                        break;
                    case knight:
                        std::cout<<"\u265E"<<"  ";
                        break;
                    case bishop:
                        std::cout<<"\u265D"<<"  ";
                        break;
                    case rook:
                        std::cout<<"\u265C"<<"  ";
                        break;
                    case queen:
                        std::cout<<"\u265B"<<"  ";
                        break;
                    case king:
                        std::cout<<"\u265A"<<"  ";
                        break;
                }
            }
            else
            {
                switch (type)
                {
                    case pawn:
                        std::cout<<"\u2659"<<"  ";
                        break;
                    case knight:
                        std::cout<<"\u2658"<<"  ";
                        break;
                    case bishop:
                        std::cout<<"\u2657"<<"  ";
                        break;
                    case rook:
                        std::cout<<"\u2656"<<"  ";
                        break;
                    case queen:
                        std::cout<<"\u2655"<<"  ";
                        break;
                    case king:
                        std::cout<<"\u2654"<<"  ";
                        break;
                }
            }
        }
        printf("\n");
    }
    printf("   A  B  C  D  E  F  G  H\n");
    return;
}

// deletes a piece from the board
// used mostly to check if a piece is pinned
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

// moves piece, only calculates the number of pins if calculatePins is true.
void Board::movePiece(int start, int end, bool calculatePins)
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

        // delete the rooks from the board occupancy
        occupancy[colour] &= ~pieces[colour][rook];
        
        // move the rook
        clear_bit(pieces[colour][rook], rookStart);
        set_bit(pieces[colour][rook], rookEnd);

        // add the rooks to the board occupancy;
        occupancy[colour] |= pieces[colour][rook];

        // recalculate total occupancy
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

    // promote pawn to queen
    if ((type == pawn && colour == white && get_bit(RANK_8, end)) ||
        (type == pawn && colour == black && get_bit(RANK_1, end)))
    {
        deletePiece(end);

        set_bit(pieces[colour][queen], end);
        set_bit(occupancy[colour], end);
        set_bit(totalOccupancy, end);

    }

    // if we are not to calculate the pins the return here
    if (!calculatePins) return;

    pinnedPieces = 0ULL;

    Board tempBoard;

    // the squares that could possibly contain pinned pieces are the squares on the diagonals and
    // straights. A piece can't be pinned by a knight
    U64 possibleWhitePins = getBishopAttacks(get_LSB(pieces[white][king]), totalOccupancy);
    possibleWhitePins |= getRookAttacks(get_LSB(pieces[white][king]), totalOccupancy);
    U64 whitePieces = occupancy[white] & possibleWhitePins;

    while (whitePieces)
    {
        // create a copy of the board
        tempBoard = *this;

        // pop the piece from the list of possibly pinned pieces
        int piece = pop_LSB(whitePieces);

        // count the number of checks the king has with the piece on the board
        int checksWithPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[white][king]), black));
        
        // remove the piece from the board
        tempBoard.deletePiece(piece);

        // count the number of checks the king has with the piece not on the board
        int checksWithoutPiece = countSetBits(tempBoard.getAttackers(get_LSB(tempBoard.pieces[white][king]), black));

        // if there are more checks with the piece than without it then it is pinned
        if (checksWithoutPiece > checksWithPiece)
        {
            set_bit(pinnedPieces, piece);
        }
    }

    // do the same for black
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

// returns the colour of the piece at a certain square, if empty then -1.
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

// returns the type of the piece at a certain square, if empty then -1.
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

// returns a bitboard with all the legal moves a piece can make
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
            // for checking of castling is a legal move:
                // check if the king is in check
                // check if the spaces between rook and king are clear.
                // check if the square the king will pass over or the
                // square the king will land in, is under attack.
            if (colour == white && !whiteKingMoved && !a1Moved
                && !getAttackers(get_LSB(pieces[colour][king]), black)
                && !get_bit(totalOccupancy, B1) && !get_bit(totalOccupancy, C1) && !get_bit(totalOccupancy, D1)
                && !getAttackers(C1, black) && !getAttackers(D1, black))
            {
                set_bit(legalMoves, C1);
            }
            else if (colour == white && !whiteKingMoved && !h1Moved
                && !getAttackers(get_LSB(pieces[colour][king]), black)
                && !get_bit(totalOccupancy, F1) && !get_bit(totalOccupancy, G1)
                && !getAttackers(F1, black) && !getAttackers(G1, black))
            {
                set_bit(legalMoves, G1);
            }
            else if (colour == black && !blackKingMoved && !a8Moved
                && !getAttackers(get_LSB(pieces[colour][king]), white)
                && !get_bit(totalOccupancy, B8) && !get_bit(totalOccupancy, C8) && !get_bit(totalOccupancy, D8)
                && !getAttackers(C8, white) && !getAttackers(D8, white))
            {
                set_bit(legalMoves, C8);
            }
            else if (colour == black && !blackKingMoved && !h8Moved
                && !getAttackers(get_LSB(pieces[colour][king]), white)
                && !get_bit(totalOccupancy, F8) && !get_bit(totalOccupancy, G8)
                && !getAttackers(F8, white) && !getAttackers(G8, white))
            {
                set_bit(legalMoves, G8);
            }
            break;
        default:
            legalMoves = 0ULL;
    }

    // subtract the occupancy of team pieces (you can't capture a team piece)
    legalMoves &= ~occupancy[colour];

    // normally the attacking squares minus the team occupancy will be all the legal moves.
    // However, if the piece is pinned or the king is in check then the legal moves will be a subset of that.
    // Finding this subset is fairly expensive which is why pinned pieces is calculated on every move
    // if the piece is pinned or there are pieces attacking the king, then check it more thourougly
    if (get_bit(pinnedPieces, p) || getAttackers(get_LSB(pieces[colour][king]), colour ? Colour::white : Colour::black))
    {

        // initialize a copy of the current legal moves
        U64 tempLegalMoves = legalMoves;

        Board tempBoard;

        // while there are moves to check
        while (tempLegalMoves)
        {
            // set the new board to the current value of the board
            tempBoard = *this;

            // pop the current legal move
            int tempMove = pop_LSB(tempLegalMoves);

            // move the piece to the current move on the copy of the board
            // the third parameter of this function call is false so movePiece doesn't
            // calculate the pins. We don't care about 2 moves in advance so it isn't necessary
            tempBoard.movePiece(p, tempMove, false);

            // if the king has attackers after making that move then remove it from the legal moves
            if (tempBoard.getAttackers(get_LSB(tempBoard.pieces[colour][king]), colour ? white : black))
            {
                clear_bit(legalMoves, tempMove);
            }

        }

    }

    return legalMoves;
}

// get all pieces that attack position "target" that have a colour "attackingColour", 
U64 Board::getAttackers(int target, int attackingColour)
{
    U64 attackers = 0ULL;
    U64 possibleAttackers = occupancy[attackingColour];

    // loop over attackers
    while (possibleAttackers)
    {
        // pop the attacker and get its type
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

// checks if the state of the game
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