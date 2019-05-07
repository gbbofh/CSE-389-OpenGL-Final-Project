EXTLIB=.
EXTINC=.
CC=g++
CFLAGS=-g -I $(EXTINC)
LIBS=-lm -lglfw -lGLEW -lGL -lassimp
LFLAGS= -L $(EXTLIB) $(LIBS)

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

all: main

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

%.o: %.cpp
	$(CC) -g -c $<


.PHONY: clean

clean:
	rm -f $(OBJ) main

