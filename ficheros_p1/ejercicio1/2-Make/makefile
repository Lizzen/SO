CC = gcc
CFLAGS = -Wall -g
LIBS = -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all : aux.o init.o aux.h
	gcc $(CFLAGS) -o prueba aux.o init.o $(LIBS)

.PHONY: clean

clean:
	-rm *.o prueba
