CC = gcc
CF = -Wall
EXEC = invertedindex

all: build
build: $(EXEC)

$(EXEC): invertedindex.o functions.o dictionar.o
	$(CC) $^ -o $@

%.o: %.c invertedindex.h
	$(CC) $(CF) -c $<

clean:
	rm *.o $(EXEC) output.out
