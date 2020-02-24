#include "PAR.h"

#include <fstream>
#include <sstream>
#include <iostream>

PAR::PAR(const std::string fichero_datos, const std::string fichero_restricciones,
	 		const int n_clusters):num_clusters(n_clusters){

	leerDatos(fichero_datos);
	leerRestricciones(fichero_restricciones);
	//clusters.size(num_clusters);


}



void PAR::leerDatos(const std::string fichero){

	std::ifstream entrada(fichero);

	if (entrada.is_open()){

		std::string cadena;

		double valor;
		std::string valor_cadena;

		int num_dato = 0;
		datos.resize(0);

		std::istringstream dato_linea;

		getline(entrada, cadena);

		while ( !entrada.eof() ){

			// cada linea, datos tendrá un dato más
			datos.resize(num_dato+1);

			dato_linea.str(cadena);

			while( !dato_linea.eof() ){

				getline(dato_linea, valor_cadena, ',');


				datos[num_dato].push_back( atof(valor_cadena.c_str()) );
			}

			dato_linea.clear();
			dato_linea.str("");

			num_dato++;

			getline(entrada, cadena);

		}


	} else {
		std::cerr << "Error abriendo " << fichero << std::endl;
	}

}


void PAR::leerRestricciones(const std::string fichero){

	std::ifstream entrada(fichero);

	if (entrada.is_open()){

		std::string linea;

		std::string valor;


		int num_linea = 0;


		std::istringstream dato_linea;

		getline(entrada, linea);

		while( !entrada.eof() ){
			dato_linea.str(linea);

			restricciones.resize(num_linea+1);


			while( !dato_linea.eof() ){

				getline(dato_linea, valor, ',');

				restricciones[num_linea].push_back(atoi(valor.c_str()));

			}

			num_linea++;


			dato_linea.clear();
			dato_linea.str("");

			getline(entrada, linea);

		}


	} else {
		std::cerr << "Error abriendo " << fichero << std::endl;
	}

}


int PAR::getNumClusters() const{
	return num_clusters;
}

void PAR::setNumClusters(const int n_num_clusters){
	num_clusters = n_num_clusters;
}
