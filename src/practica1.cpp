#include <iostream>

#include "PAR.h"
#include "random.h"
#include <fstream>

#include "timer.h"

void ejecutar_PAR(PAR & par,const std::string datos, const std::string restricciones,
	               const int clusters, const unsigned seed, const std::string alg){



	std::pair<std::vector<PAR::Cluster>,double> solucion;

	std::cout << std::endl << "Solución " << alg << " para " << datos << " con restricciones de " << restricciones << " con semilla " << seed << std::endl << std::endl << std::flush;

	std::fstream fichero;

	std::string path = restricciones + "_" + std::to_string(seed)  + "_" + alg +  ".out";

	fichero.open (path, std::fstream::out);

	double tiempo = 0.0d;

	unsigned evaluaciones = 100000;
	unsigned tam_pob_ini = 50;

	start_timers();
	if (alg == "BL"){
		int eval_BL = 100000;
		solucion = par.algoritmo_BL(par.generar_solucion_aleatoria(), eval_BL, true);
		//solucion = par.algoritmo_BL(par.algoritmo_greedy().first)
	} else if (alg == "GREEDY") {
		solucion = par.algoritmo_greedy();
	} else if (alg == "AGG-SF"){
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::SEGMENTO_FIJO, tipo_generacion::GENERACIONAL);
	} else if (alg == "AGG-UN"){
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::GENERACIONAL );
	} else if (alg == "AGE-SF"){
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 1, operador_cruce::SEGMENTO_FIJO, tipo_generacion::ESTACIONARIO);
	} else if (alg == "AGE-UN"){
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 1, operador_cruce::UNIFORME, tipo_generacion::ESTACIONARIO );
	} else if (alg == "AM-10-1"){
		tam_pob_ini = 10;
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::MEMETICO_1 );
	} else if (alg == "AM-10-0_1"){
		tam_pob_ini = 10;
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::MEMETICO_0_1 );
	} else if (alg == "AM-10-0_1mej"){
		tam_pob_ini = 10;
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::MEMETICO_0_1_MEJ );
	} else if (alg == "AM-BL-10-1"){
		evaluaciones = 500000;
		tam_pob_ini = 50;
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::MEMETICO_BL_1 );
	} else if (alg == "AM-BL-10-0_1"){
		evaluaciones = 500000;
		tam_pob_ini = 50;
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::MEMETICO_BL_0_1 );
	} else if (alg == "AM-BL-10-0_1mej"){
		tam_pob_ini = 50;
		evaluaciones = 500000;
		solucion = par.algoritmos_AG(evaluaciones, tam_pob_ini, 0.001, 0.7, operador_cruce::UNIFORME, tipo_generacion::MEMETICO_BL_0_1_MEJ );
	}


	tiempo = elapsed_time();

	if (solucion.first.size() != 0){
		fichero << "Tiempo:" << std::endl << tiempo << std::endl << std::endl;
		fichero << par;

		//std::cout << par;
	}



}

int main(int argc, char ** argv){



	if (argc < 3){
		std::cerr << "ERROR: Faltan argumentos:" << std::endl
			  		 << "\t USO: " << argv[0] << " <fichero_datos> <fichero_restricciones> <num_clusters> <semilla> " << std::endl;
		exit(-1);
	}

	std::string datos = argv[1];
	std::string restricciones = argv[2];
	int clus = unsigned(atoi(argv[3]));
	int semilla = unsigned(atoi(argv[4]));


	PAR par(datos, restricciones, clus, std::to_string(semilla));

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "GREEDY");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "BL");


	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AM-10-1");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AM-10-0_1");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AM-10-0_1mej");


	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AGG-UN");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AGG-SF");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AGE-UN");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AGE-SF");


	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AM-BL-10-1");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AM-BL-10-0_1");

	Set_random( semilla );

	ejecutar_PAR(par, datos, restricciones, clus, semilla, "AM-BL-10-0_1mej");

	return 0;
}
