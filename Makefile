IDIR = include
ODIR = obj
CC = gcc
CFLAGS = -O2 -Wall -pedantic -I$(IDIR) -std=c11 -Werror
LIBS = -lcurses -lm
EXEC = game

_DEPS = arraylist.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ = arraylist.o game.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean: 
	rm -f $(ODIR)/*.o
	rm -f ./$(EXEC)

