#include <iostream>

#include "PAR.h"
#include "random.h"


int main(int argc, char ** argv){
	PAR par("datos/ecoli_set.dat", "datos/ecoli_set_const_10.const", 3);


	int sol = 0;
	int ejecuciones = 0;

	if (argc > 1){
		ejecuciones = atoi(argv[1]);
	} else {
		ejecuciones = 5;
	}

	std::vector<PAR::Cluster> solucion;

	for (int i = 0; i < ejecuciones; i++){

		solucion = par.algoritmo_COPKM();
		if (solucion.size() != 0){
			sol++;

			std::cout << "Solución para la ejecución " << i << std::endl;
			for (int i = 0; i < solucion.size(); i++){
				std::cout << "Cluster " << i << ": " << std::endl;
				for (auto it = solucion[i].get_elementos().begin(); it != solucion[i].get_elementos().end(); ++it){
					std::cout << (*it) << " ";
				}

				std::cout << std::endl;
			}

			std::cout << std::endl << std::endl;
		}

	}

	std::cout << "Ejecuciones del algoritmo: " << ejecuciones << std::endl;
	std::cout << "Soluciones encontradas: " << sol << std::endl;


	return 0;
}
