HOME     = .
BIN      = $(HOME)/bin
INCLUDE  = $(HOME)/include
SRC      = $(HOME)/src
OBJ      = $(HOME)/obj
LIB      = $(HOME)/lib
DATOS	   = $(HOME)/datos
GRAFICAS = $(HOME)/graficas/datos

FLAGS = --std=c++17 -O3 -Wall

all: $(BIN)/practica1


$(BIN)/practica1: $(OBJ)/PAR.o $(OBJ)/practica1.o $(OBJ)/random.o $(OBJ)/timer.o
	g++ $^ -o $@

$(OBJ)/practica1.o: $(SRC)/practica1.cpp
	g++ -c $(FLAGS) $^ -I$(INCLUDE) -o $@


$(OBJ)/PAR.o: $(SRC)/PAR.cpp
	g++ -c $(FLAGS) $^ -I$(INCLUDE) -o $@

$(OBJ)/random.o: $(SRC)/random.cpp
	g++ -c $(FLAGS) $^ -I$(INCLUDE) -o $@

$(OBJ)/timer.o: $(SRC)/timer.c
	g++ -c $(FLAGS) $^ -I$(INCLUDE) -o $@




clean:
	-rm $(OBJ)/*.o
	-rm $(BIN)/*

limpiar-salida:
	-rm $(DATOS)/*.out
	-rm $(GRAFICAS)/*.out
