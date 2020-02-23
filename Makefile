HOME    = .
BIN     = $(HOME)/bin
INCLUDE = $(HOME)/include
SRC     = $(HOME)/src
OBJ     = $(HOME)/obj
LIB     = $(HOME)/lib

FLAGS = --std=c++11

all: $(BIN)/practica1


$(BIN)/practica1: $(OBJ)/PAR.o $(OBJ)/practica1.o
	g++ $(OBJ)/practica1.o $(OBJ)/PAR.o -o $(BIN)/practica1

$(OBJ)/practica1.o: $(SRC)/practica1.cpp $(INCLUDE)/PAR.h
	g++ -c $(FLAGS) $(SRC)/practica1.cpp -I$(INCLUDE) -o $(OBJ)/practica1.o


$(OBJ)/PAR.o: $(SRC)/PAR.cpp $(INCLUDE)/PAR.h
	g++ -c $(FLAGS) $(SRC)/PAR.cpp -I$(INCLUDE) -o $(OBJ)/PAR.o



clean:
	-rm $(OBJ)/*.o
	-rm $(BIN)/*
