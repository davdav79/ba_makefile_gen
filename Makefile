.DEFAULT_GOAL := all
.PHONY: all
CC=gcc
CFLAGS=-pthread
DEPS=test1.h
OBJ=main.c test1.c


all: main
	./main

main: $(OBJ) $(DEPS)
	$(CC) -o main $(OBJ) $(CFLAGS)
