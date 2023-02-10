.DEFAULT_GOAL := all
.PHONY: all
CFLAGS= -W -Wall -Wextra -fstack-protector-all

all: makegen

####################MAKEGEN####################

MAKEGENOBJ= ./argp_helper.c ./dot_helper.c ./tree.c ./string_helper.c ./file_helper.c ./make_helper.c ./linked_list.c 

MAKEGENDEP= ./dot_helper.h ./tree.h ./string_helper.h ./file_helper.h ./make_helper.h 

MAKEGENLIB= -lpthread

makegen: $(MAKEGENOBJ) $(MAKEGENLIB)

clean:
	$(RM) main2 main3 makegen *.o *.png *.dot

test: clean makegen 
	./makegen  -p "." -c -o test_file