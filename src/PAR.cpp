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

	leer_datos(fichero_datos);
	leer_restricciones(fichero_restricciones);

	for (int i = 0; i < num_clusters; i++){
		clusters.push_back( Cluster((*this)) );
	}


}



void PAR::leer_datos(const std::string fichero){

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


void PAR::leer_restricciones(const std::string fichero){

	std::ifstream entrada(fichero);

	if (entrada.is_open()){

		std::string linea;

		std::string valor;


		int num_linea = 0;
		int num_elemento = 0;

		int v_res;


		std::istringstream dato_linea;

		getline(entrada, linea);

		while( !entrada.eof() ){
			dato_linea.str(linea);

			//restricciones.resize(num_linea+1);

			num_elemento = 0;

			while( !dato_linea.eof() && num_elemento < num_linea ){

				getline(dato_linea, valor, ',');

				v_res = atoi(valor.c_str());

				//restricciones[num_linea].push_back(atoi(valor.c_str()));

				if (v_res != 0)
					restricciones.insert( std::make_pair( std::make_pair(num_linea, num_elemento), v_res ));

				num_elemento++;

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


int PAR::get_num_clusters() const{
	return num_clusters;
}

void PAR::set_num_clusters(const int n_num_clusters){
	num_clusters = n_num_clusters;
}


std::vector<PAR::Cluster> PAR::algoritmo_COPKM(){
	std::vector<int> indices;

	for (int i = 0; i < datos.size(); i++){
		indices.push_back(i);
	}

	std::srand( unsigned( std::time(0) ) );

	std::random_shuffle(indices.begin(), indices.end());


	bool hay_cambios = false;

	int num_cluster;

	do {

		for (int i = 0; i < indices.size(); i++){
			 num_cluster = buscar_cluster(indices[i]);

			 if (num_cluster == -1){
				 return std::vector<PAR::Cluster>();
			 } else {
				 clusters[num_cluster].add_elemento(indices[i]);
			 }
		}

		for (int i = 0; i < num_clusters; i ++){
			clusters[i].calcular_centroide();
		}

	} while(hay_cambios);


	return clusters;

}

int PAR::buscar_cluster(const int elemento){

	for (int i = 0; i < num_clusters; i++){

	}


	return 0;
}












/*

Clase Cluster

*/

PAR::Cluster::Cluster( PAR & p ):problema(p){

}


/*
 Función para calcular el centroide de un cluster

 PRE: El cluster tiene al menos un elemento

*/

void PAR::Cluster::calcular_centroide(){


	auto it = elementos.begin();

	centroide.resize(problema.datos[(*it)].size());

	++it;

	for (int i = 0; i < centroide.size(); i++){

		it = elementos.begin();


		for (int j = 0; j < elementos.size(); j++){
			centroide[i] += problema.datos[(*it)][i];

			++it;

		}

		centroide[i] /= problema.datos[(*it)].size();

	}

}


void PAR::Cluster::calcular_distancia_intra_cluster(){
	auto it = elementos.begin();

	distancia_intra_cluster = std::vector<double>(centroide.size(), 0);


}

void PAR::Cluster::set_centroide(const std::vector<double> n_centroide){

}


std::vector<double> PAR::Cluster::get_centroide() const{
	return centroide;
}

std::set<int> PAR::Cluster::get_elementos() const{
	return elementos;
}


void PAR::Cluster::add_elemento(const int elemento){

}

void PAR::Cluster::delete_elemento(const int elemento){

}
