.DEFAULT_GOAL := all
.PHONY: all
CFLAGS= -W -Wall -Wextra

all: makegen

####################MAIN####################
MAINOBJ= ./string_helper.c ./file_helper.c ./dot_helper.c ./make_helper.c  ./test1.c  ./test/test1-Copy.c ./linked_list.c ./tree.c 

makegen: $(MAINOBJ)

clean:
	 $(RM) main main2 main3 *.o

test: makegen
	./makegen