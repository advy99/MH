#include <iostream>

#include "PAR.h"



int main(int argc, char ** argv){
	PAR par("datos/iris_set.dat", "datos/iris_set_const_10.const", 3);

	int sol = 0;
	int ejecuciones = 0;

	if (argc > 1){
		ejecuciones = atoi(argv[1]);
	} else {
		ejecuciones = 1000;
	}


	for (int i = 0; i < ejecuciones; i++){
		if (par.algoritmo_COPKM().size() != 0){
			sol++;
		}

	}

	std::cout << "Ejecuciones del algoritmo: " << ejecuciones << std::endl;
	std::cout << "Soluciones encontradas: " << sol << std::endl;


	return 0;
}
