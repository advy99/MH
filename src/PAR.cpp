#include "PAR.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

/*

Clase PAR

*/

PAR::PAR(const std::string fichero_datos, const std::string fichero_restricciones,
	 		const int n_clusters):num_clusters(n_clusters){

	leerDatos(fichero_datos);
	leerRestricciones(fichero_restricciones);
	clusters.resize(num_clusters);


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


std::vector<Cluster> PAR::algoritmoCOPKM(){
	std::vector<int> indices;

	for (int i = 0; i < datos.size(); i++){
		indices.push_back(i);
	}

	std::srand ( unsigned ( std::time(0) ) );

	std::random_shuffle(indices.begin(), indices.end());


	bool hay_cambios = false;

	int num_cluster;

	do {

		for (int i = 0; i < indices.size(); i++){
			 num_cluster = buscarCluster(datos[indices[i]]);

			 if (num_cluster == -1){
				 return std::vector<Cluster>;
			 } else {
				 clusters[num_cluster].addElemento(indices[i]);
			 }
		}

		for (int i = 0; i < num_clusters; i ++){
			clusters[i].calcularCentroide();
		}

	} while(hay_cambios);

}















/*

Clase Cluster

*/

PAR::Cluster::Cluster(){

}


/*
 Función para calcular el centroide de un cluster

 PRE: El cluster tiene al menos un elemento

*/

void PAR::Cluster::calcularCentroide(){


	auto it = elementos.begin();

	centroide.resize((*it).size());

	++it;

	for (int i = 0; i < centroide.size(); i++){

		it = elementos.begin();


		for (int j = 0; j < elementos.size(); j++){
			centroide[i] += datos[(*it)][i];

			++it;

		}

		centroide[i] /= datos[(*it)].size();

	}

}


void PAR::Cluster::calcularDistanciaIntraCluster(){
	auto it = elementos.begin();

	distancia_intra_cluster = std::vector<double>(centroide.size(), 0);


}

void PAR::Cluster::setCentroide(const std::vector<double> n_centroide){

}


std::vector<double> PAR::Cluster::getCentroide() const{
	return centroide;
}

std::set<int> PAR::Cluster::getElementos() const{
	return elementos;
}


void PAR::Cluster::addElemento(const int elemento){

}

void PAR::Cluster::deleteElemento(const int elemento){

}
