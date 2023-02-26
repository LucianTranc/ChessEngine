CC = g++
CFLAGS = -Wall -std=c++11

all: Board.o ChessAI.o main.o
	$(CC) $(CFLAGS) bin/Board.o bin/ChessAI.o bin/main.o -o play

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o bin/main.o

Board.o: Board.cpp Board.hpp bitboard.hpp MoveGenerator.hpp
	$(CC) $(CFLAGS) -c Board.cpp -o bin/Board.o

ChessAI.o: ChessAI.cpp ChessAI.hpp
	$(CC) $(CFLAGS) -c ChessAI.cpp -o bin/ChessAI.o

clean :
	-rm *.o bin/*.o play