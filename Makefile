CC=gcc
CFLAGS=-DDEBUG -Wall -pedantic -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11
#-Werror
OBJECTS=Xerxes.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: all
all: Xerxes

Xerxes: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	rm *.o
