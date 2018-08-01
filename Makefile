CC = gcc
CFLAGS = -O2 -Wall -pedantic -I. -std=c11
LIBS = -lcurses -lm

BUILD_DIR = ./bin

game: game.c;
	mkdir -p $(BUILD_DIR)
	$(CC) game.c -o $(BUILD_DIR)/game $(CFLAGS) $(LIBS)

.PHONY: clean

clean: 
	rm -rf $(BUILD_DIR)/*

