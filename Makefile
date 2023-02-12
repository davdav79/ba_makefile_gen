.DEFAULT_GOAL := all
.PHONY: all
CFLAGS= -W -Wall -Wextra

all: makegen 

####################MAKEGEN####################

MAKEGENOBJ= ./argp_helper.o ./dot_helper.o ./file_helper.o ./string_helper.o ./linked_list.o ./make_helper.o ./tree.o 

MAKEGENLIB= 

makegen: $(MAKEGENOBJ) $(MAKEGENLIB)

clean:
	$(RM) makegen makegen.png *.dot *.o Makefile.bak