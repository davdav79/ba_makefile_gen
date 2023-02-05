.DEFAULT_GOAL := all
.PHONY: all
CFLAGS= -W -Wall -Wextra

all: makegen

####################MAKEGEN####################

MAKEGENOBJ= ./dot_helper.c ./tree.c ./string_helper.c ./file_helper.c ./make_helper.c ./linked_list.c 

MAKEGENDEP= ./dot_helper.h ./tree.h ./string_helper.h ./file_helper.h ./make_helper.h 

makegen: $(MAKEGENOBJ)

clean:
	$(RM) main2 main3 makegen *.o

test: clean makegen 
	./makegen