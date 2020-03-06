#include <iostream>

#include "PAR.h"
#include "random.h"


void ejecutar_PAR_greedy(const std::string datos, const std::string restricciones,
	               		 const int clusters, const int iteraciones){

	PAR par(datos, restricciones, clusters);


	std::pair<std::vector<PAR::Cluster>,int> solucion;

	std::cout << std::endl << "Solución Greedy para " << datos << " con restricciones de " << restricciones << std::endl << std::endl; 

	for (int i = 0; i < iteraciones; i++){

		solucion = par.algoritmo_greedy();
		if (solucion.first.size() != 0){

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

}

int main(int argc, char ** argv){



	int ejecuciones = 0;

	if (argc > 1){
		ejecuciones = atoi(argv[1]);
	} else {
		ejecuciones = 5;
	}

	Set_random( unsigned(1591999) );

	ejecutar_PAR_greedy("datos/iris_set.dat", "datos/iris_set_const_10.const", 3, ejecuciones);
	ejecutar_PAR_greedy("datos/iris_set.dat", "datos/iris_set_const_20.const", 3, ejecuciones);

	Set_random( unsigned(1591999) );

	ejecutar_PAR_greedy("datos/rand_set.dat", "datos/rand_set_const_10.const", 3, ejecuciones);
	ejecutar_PAR_greedy("datos/rand_set.dat", "datos/rand_set_const_20.const", 3, ejecuciones);


	Set_random( unsigned(1591999) );

	ejecutar_PAR_greedy("datos/ecoli_set.dat", "datos/ecoli_set_const_10.const", 3, ejecuciones);
	ejecutar_PAR_greedy("datos/ecoli_set.dat", "datos/ecoli_set_const_20.const", 3, ejecuciones);



	return 0;
}
