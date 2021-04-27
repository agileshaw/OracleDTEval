CC = gcc
CFLAGS = -Wall -g

all: generator average
average: average.c
	$(CC) $(CFLAGS) -o average average.c
generator: main.o rules.o data.o node.o prob.o
	$(CC) $(CFLAGS) -o generator main.o rules.o data.o node.o prob.o -lm
main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.c -lm
rules.o: rules.c rules.h
	$(CC) $(CFLAGS) -c rules.c -lm
data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c -lm
node.o: node.c node.h
	$(CC) $(CFLAGS) -c node.c -lm
prob.o: prob.c prob.h
	$(CC) $(CFLAGS) -c prob.c -lm

clean:
	rm *.o 
	rm -rf *.dSYM
.PHONY:
	classifier
