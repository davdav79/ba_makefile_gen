.DEFAULT_GOAL := all
.PHONY: all
CC=gcc

####################MAIN####################
MAINCFLAGS=
MAINDEPS= ./test1.h ./test/test1-Copy.h ./test/wasd/wasd.h
MAINOBJ= ./main.c ./test1.c ./test/test1-Copy.c

main: $(MAINOBJ) $(MAINDEPS)
	$(CC) -o MAIN $(MAINOBJ) $(MAINCFLAGS)

all: main