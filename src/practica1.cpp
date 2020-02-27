#include <iostream>

#include "PAR.h"



int main(int argc, char ** argv){
	PAR par("datos/rand_set.dat", "datos/rand_set_const_10.const", 8);

	par.algoritmo_COPKM();

	return 0;
}
