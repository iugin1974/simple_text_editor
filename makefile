CC = g++
FLAGS = -g -Wall
LIBS = -lncurses
OBJ = Buffer.cpp View.cpp Controller.cpp
#OBJ = main.cpp Buffer.cpp
te: $(OBJ)
	$(CC) $(FLAGS) -o te $(OBJ) $(LIBS)

install:
	cp te /usr/bin
