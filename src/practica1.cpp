#include <iostream>

#include "PAR.h"
#include "random.h"
#include <fstream>


void ejecutar_PAR_greedy(const std::string datos, const std::string restricciones,
	               		 const int clusters, const unsigned seed ){

	PAR par(datos, restricciones, clusters);


	std::pair<std::vector<PAR::Cluster>,int> solucion;

	std::cout << std::endl << "Solución Greedy para " << datos << " con restricciones de " << restricciones << " con semilla " << seed << std::endl << std::endl << std::flush;

	std::fstream fichero;

	std::string path = restricciones + "_" + std::to_string(seed)  + "_GREEDY.out";

	fichero.open (path, std::fstream::out);

	solucion = par.algoritmo_greedy();
	if (solucion.first.size() != 0){
		fichero << par;
		std::cout << par;
	}


}


void ejecutar_PAR_BL(const std::string datos, const std::string restricciones,
	               		 const int clusters, const unsigned seed){

	PAR par(datos, restricciones, clusters);


	std::pair<std::vector<PAR::Cluster>,int> solucion;

	std::cout << std::endl << "Solución BL para " << datos << " con restricciones de " << restricciones << " con semilla " << seed << std::endl << std::endl << std::flush;

	std::fstream fichero;

	std::string path = restricciones + "_" + std::to_string(seed)  + "_BL.out";

	fichero.open (path, std::fstream::out);

	solucion = par.algoritmo_BL();
	if (solucion.first.size() != 0){
		fichero << par;

		std::cout << par;
	}



}

int main(int argc, char ** argv){



	int ejecuciones = 0;

	if (argc < 3){
		std::cerr << "ERROR: Faltan argumentos:" << std::endl
			  		 << "\t USO: " << argv[0] << " <fichero_datos> <fichero_restricciones> <num_clusters> <semilla> " << std::endl;
		exit(-1);
	}

	std::string datos = argv[1];
	std::string restricciones = argv[2];
	int clus = unsigned(atoi(argv[3]));
	int semilla = unsigned(atoi(argv[4]));

	Set_random( semilla );

	ejecutar_PAR_greedy(datos, restricciones, clus, semilla);

	ejecutar_PAR_BL(datos, restricciones, clus, semilla);



	return 0;
}
