#include <iostream>

#include "PAR.h"
#include "random.h"


int main(int argc, char ** argv){
	PAR par("datos/iris_set.dat", "datos/iris_set_const_10.const", 3);


	int sol = 0;
	int ejecuciones = 0;

	if (argc > 1){
		ejecuciones = atoi(argv[1]);
	} else {
		ejecuciones = 5;
	}

	std::pair<std::vector<PAR::Cluster>,int> solucion;

	for (int i = 0; i < ejecuciones; i++){

		solucion = par.algoritmo_BL();
		if (solucion.first.size() != 0){
			sol++;

			std::cout << "Solución para la ejecución " << i << std::endl;
			std::cout << "Infactibilidad: " << solucion.second << std::endl;

			for (unsigned j = 0; j < solucion.first.size(); j++){
				std::cout << "Cluster " << j << ": " << std::endl;
				for (auto it = solucion.first[j].get_elementos().begin(); it != solucion.first[j].get_elementos().end(); ++it){
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
