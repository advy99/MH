#include "PAR.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "random.h"

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

	std::srand( unsigned( std::time(0) ) );
	Set_random(std::time(0));

	//std::srand( unsigned(15091999) );
	//Set_random( unsigned(15091999) );

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

			while( !dato_linea.eof() /*&& num_elemento < num_linea */){

				getline(dato_linea, valor, ',');

				v_res = atoi(valor.c_str());

				//restricciones[num_linea].push_back(atoi(valor.c_str()));

				if (v_res != 0 && num_linea != num_elemento)
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

	for (int i = 0; i < num_clusters; i++){
		std::vector<double> n_centroide;
		for (int j = 0; j < clusters[i].get_centroide().size(); j++){
			n_centroide.push_back(Randfloat(0.0, 100.0));
		}
		clusters[i].limpiar();
		clusters[i].set_centroide(n_centroide);
	}



	std::random_shuffle(indices.begin(), indices.end(), RandPositiveInt);


	bool hay_cambios = false;

	int num_cluster;

	std::vector<double> centroide_antiguo;

	std::vector<Cluster> n_sol = clusters;


	do {

		hay_cambios = false;

		// no hace falta ++it, porque los vamos borrando y se va actualizando al
		// siguiente cada vez que borramos uno
		for (auto it = indices.begin(); it != indices.end(); ++it){
			 num_cluster = buscar_cluster( (*it) );

			 if (num_cluster == -1){
				 return std::vector<PAR::Cluster>();
			 } else {
				 clusters[num_cluster].add_elemento( (*it) );
			 }
		}


		for (int i = 0; i < num_clusters; i++){

			if (n_sol[i].get_elementos() != clusters[i].get_elementos()){
				hay_cambios = true;
			}

			//centroide_antiguo = clusters[i].get_centroide();
			clusters[i].calcular_centroide();
			//if (centroide_antiguo != clusters[i].get_centroide()){
			//	hay_cambios = true;
			//}

			n_sol[i] = clusters[i];

			clusters[i].limpiar();

		}


	} while(hay_cambios);



	return clusters;

}

int PAR::buscar_cluster(const int elemento){

	double d;
	double menor_distancia = std::numeric_limits<double>::infinity();
	int cluster_menor_distancia = -1;


	for (int i = 0; i < num_clusters; i++){

		//if (cumple_restricciones(elemento, i)){
			d = distancia_puntos(clusters[i].get_centroide(), datos[elemento]);

			if (d < menor_distancia){
				menor_distancia = d;
				cluster_menor_distancia = i;
			}

		//}

	}

	return cluster_menor_distancia;
}


std::vector<int> PAR::clusters_to_solucion() {
	std::vector<int> solucion;
	solucion.resize(num_clusters);

	for (int i = 0; i < clusters.size(); i++){
		for (auto it = clusters[i].get_elementos().begin();
		     it != clusters[i].get_elementos().end(); ++it ){
			solucion[(*it)] = i;
		}
	}

	return solucion;
}


double PAR::distancia_puntos(const std::vector<double> & p1,
									  const std::vector<double> & p2){


	double distancia = 0;

	for (int i = 0; i < p1.size(); i++){
		distancia += std::abs(p1[i] - p2[i]) * std::abs(p1[i] - p2[i]);
	}



	return distancia;
}


bool PAR::cumple_restricciones(const int elemento, const int cluster){

	bool las_cumple = true;

	// para el cluster dado, iteramos sobre todos sus elementos
	for (auto it = clusters[cluster].get_elementos().begin();
		  it != clusters[cluster].get_elementos().end() && las_cumple; ++it){

		// buscamos la restricción asociada al elemento dado con todos los elementos
		// pertenecientes al cluster
		auto pos = restricciones.find(std::make_pair(elemento, (*it)));

		// si encuentra una restricción asociada y es que no pueden ir juntos,
		// quiere decir que no cumple dichas restricciones
		if (pos != restricciones.end() && (*pos).second == -1){
			las_cumple = false;
		}

 	}



	// si las sigue cumpliendo
	if (las_cumple)
		// para todos los demas clusters
		for (int i = 0; i < num_clusters; i++){
			if (i != cluster){
				// buscamos si es obigatorio que tengan que ir juntos
				for (auto it = clusters[i].get_elementos().begin();
					  it != clusters[i].get_elementos().end() && las_cumple; ++it){

					auto pos = restricciones.find(std::make_pair(elemento, (*it)));
					// si tienen que ir juntos, no cumple las restricciones ya que estamos
					// intentando meterlo en un cluster distinto
					if (pos != restricciones.end() && (*pos).second == 1)
						las_cumple = false;
				}
			}

		}


	return las_cumple;
}





















/*

Clase Cluster

*/

PAR::Cluster::Cluster( PAR & p ):problema(p){
	centroide = std::vector<double>(p.datos[0].size(), 0);
}


/*
 Función para calcular el centroide de un cluster

 PRE: El cluster tiene al menos un elemento

*/

void PAR::Cluster::calcular_centroide(){

	for(auto it = elementos.begin(); it != elementos.end(); ++it){
		for (int j = 0; j < problema.datos[(*it)].size(); j++){
			centroide[j] += problema.datos[(*it)][j];
		}
	}

	for (int i = 0; i < centroide.size(); i++){
		centroide[i] /= centroide.size();
	}

}


void PAR::Cluster::calcular_distancia_intra_cluster(){
	auto it = elementos.begin();

	distancia_intra_cluster = std::vector<double>(centroide.size(), 0);


}

void PAR::Cluster::set_centroide(const std::vector<double> n_centroide){
	centroide = n_centroide;;
}


const std::vector<double> & PAR::Cluster::get_centroide() const{
	return centroide;
}

const std::set<int> & PAR::Cluster::get_elementos() const{
	return elementos;
}


void PAR::Cluster::add_elemento(const int elemento){
	elementos.insert(elemento);
}

void PAR::Cluster::delete_elemento(const int elemento){
	elementos.erase(elementos.find(elemento));
}

void PAR::Cluster::limpiar(){
	elementos.clear();
}

PAR::Cluster & PAR::Cluster::operator=(const PAR::Cluster & otro){
	this->centroide = otro.centroide;
	this->distancia_intra_cluster = otro.distancia_intra_cluster;
	this->elementos = otro.elementos;
	this->problema = otro.problema;

	return *this;
}
