CC=gcc
CFLAGS=-Wall -Wextra -pedantic -ggdb
LIBS=-lncurses
SRC=src/*.c
BIN=snake

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LIBS)
