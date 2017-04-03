CFLAGS=-std=gnu99 -pedantic -Wall -Wextra -Werror
FLAGS=-lpthread -lrt
PROGS=rivercrossing

all: $(PROGS)

rivercrossing: rivercrossing.c rivercrossing.h
	gcc $(CFLAGS) -o rivercrossing rivercrossing.c $(FLAGS)


clean:
	rm -f *.o $(PROGS)
