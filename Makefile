.DEFAULT_GOAL := all
.PHONY: all
CFLAGS= -W -Wall -Wextra

all: demo makegen 

####################DEMO####################

DEMOOBJ= ./linked_list.o ./tree.o ./string_helper.o 

DEMOLIB= -lpthread 

demo: $(DEMOOBJ) $(DEMOLIB)

####################MAKEGEN####################

MAKEGENOBJ= ./argp_helper.o ./make_helper.o ./dot_helper.o ./file_helper.o ./string_helper.o ./linked_list.o ./tree.o 

MAKEGENLIB= 

makegen: $(MAKEGENOBJ) $(MAKEGENLIB)

clean:
	$(RM) demo demo.png makegen makegen.png *.dot *.o Makefile.bak