CC = g++
CFLAGS = -Wall -std=c++11

all: main.cpp
	$(CC) $(CFLAGS) main.cpp -o play
	
clean :
	-rm *.o bin/*.o play