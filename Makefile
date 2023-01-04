.DEFAULT_GOAL := all
.PHONY: all
CC=gcc
CFLAGS=-pthread
DEPS = test1.h
OBJ = test1.c main.c


all:
	$(CC) -o main $(OBJ) $(CFLAGS)
	./main
