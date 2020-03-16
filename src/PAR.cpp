#include "PAR.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "random.h"
#include <cmath>




/*

Clase PAR

*/

PAR::PAR(const std::string fichero_datos, const std::string fichero_restricciones,
	 		const int n_clusters){

	leer_datos(fichero_datos);
	leer_restricciones(fichero_restricciones);


	// creamos todos
	for (int i = 0; i < n_clusters; i++){
		clusters.push_back( Cluster((*this)) );
	}

	double d;

	mayor_distancia = 0;

	for (unsigned i = 0; i < datos.size() - 1; i++){
		for (unsigned j = i; j < datos.size(); j++){
			d = distancia_puntos(datos[i], datos[j]);
			if (d > mayor_distancia){
				mayor_distancia = d * 1.0;
			}

		}
	}

	std::cout << "Mayor distancia " << mayor_distancia << std::endl;



}



void PAR::leer_datos(const std::string fichero){

	std::ifstream entrada(fichero);

	if (entrada.is_open()){

		std::string cadena;

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
	return clusters.size();
}

double PAR::get_desviacion_general() const{
	return desviacion_general;
}



std::pair<std::vector<PAR::Cluster>,int> PAR::algoritmo_greedy(){


	// inicialización de indices aleatorios
	std::vector<int> indices;

	for (unsigned i = 0; i < datos.size(); i++){
		indices.push_back(i);
	}

	std::random_shuffle(indices.begin(), indices.end(), RandPositiveInt);


	double minimo = std::numeric_limits<double>::infinity();

	double maximo = std::numeric_limits<double>::infinity();
	maximo = -maximo;


	for (unsigned i = 0; i < datos.size(); i++){
		for (unsigned j = 0; j < datos[i].size(); j++){
			if (datos[i][j] < minimo){
				minimo = datos[i][j];
			}
			if (datos[i][j] > maximo){
				maximo = datos[i][j];
			}
		}
	}

	// inicialización de centroides aleatorios
	for (unsigned i = 0; i < clusters.size(); i++){
		std::vector<double> n_centroide;
		for (unsigned j = 0; j < clusters[i].get_centroide().size(); j++){
			n_centroide.push_back(Randfloat(minimo, maximo));
		}
		clusters[i].limpiar();
		clusters[i].set_centroide(n_centroide);
	}



	bool hay_cambios = false;
	std::vector<bool> cambios(clusters.size(), false);

	int num_cluster;

	std::vector<Cluster> n_sol = clusters;

	do {

		infactibilidad = 0;

		hay_cambios = false;
		cambios = std::vector<bool>(clusters.size(), false);

		for (auto it = indices.begin(); it != indices.end(); ++it){

			 num_cluster = buscar_cluster( (*it) );

			 clusters[num_cluster].add_elemento( (*it) );

		}


		for (unsigned i = 0; i < clusters.size(); i++){

			cambios[i] = n_sol[i].get_elementos() != clusters[i].get_elementos();


			if (cambios[i]){
				clusters[i].calcular_centroide();

				n_sol[i] = clusters[i];
			}


			clusters[i].limpiar();

		}

		for (unsigned i = 0; i < cambios.size(); i++){
			hay_cambios = hay_cambios || cambios[i];
		}

	} while(hay_cambios);


	clusters = n_sol;

	calcular_desviacion_general();

	infactibilidad = calcular_infactibilidad();

	return std::make_pair(clusters, infactibilidad);

}

int PAR::buscar_cluster(const int elemento){

	double d;
	double menor_distancia = std::numeric_limits<double>::infinity();
	int cluster_menor_distancia = -1;
	int menor_restricciones = restricciones.size();


	std::vector< std::pair<int, int>> aumento_infactibilidad;


	for (unsigned i = 0; i < clusters.size(); i++){

		aumento_infactibilidad.push_back( std::make_pair(cumple_restricciones(elemento, i), i ) );

	}

	std::sort(aumento_infactibilidad.begin(), aumento_infactibilidad.end());

	menor_restricciones = aumento_infactibilidad.begin()->first;

	for (auto it = aumento_infactibilidad.begin(); it->first == menor_restricciones && it != aumento_infactibilidad.end(); ++it){

		d = distancia_puntos(clusters[(*it).second].get_centroide(), datos[elemento]);

		if (d <= menor_distancia){
			menor_distancia = d;
			cluster_menor_distancia = (*it).second;
		}

	}

	infactibilidad += menor_restricciones;

	return cluster_menor_distancia;
}


std::vector<int> PAR::clusters_to_solucion() {

	std::vector<int> solucion;

	int tam_total = 0;

	for (unsigned i = 0; i < clusters.size(); i++){
		tam_total += clusters[i].num_elementos();
	}

	solucion.resize(tam_total);

	for (unsigned i = 0; i < clusters.size(); i++){
		for (auto it = clusters[i].get_elementos().begin();
		     it != clusters[i].get_elementos().end(); ++it ){
			solucion[(*it)] = i;
		}
	}

	return solucion;
}


/**
	@brief Función para calcular la distancia entre dos puntos

	@param p1 Punto 1
	@param p2 Punto 2

	@return double Distancia entre dos puntos
*/

double PAR::distancia_puntos(const std::vector<double> & p1,
									  const std::vector<double> & p2) const{


	double distancia = 0;

	// hacemos la distancia euclidea de los dos puntos
	// de momento no hacemos la raiz cuadrada, operacion demasiado costosa
	// y en realidad nos da igual al comparar
	for (unsigned i = 0; i < p1.size(); i++){
		distancia += std::abs(p1[i] - p2[i]) * std::abs(p1[i] - p2[i]);
	}


	return distancia;
}


/**
	@brief Función para calcular el número de restricciones que no son cumplidas

	@param elemento Elemento que queremos insertar en los clusters
	@param

	@return int restricciones incumplidas
*/

int PAR::cumple_restricciones(const int elemento, const int cluster){

	int incumplidas = 0;

	// para el cluster dado, iteramos sobre todos sus elementos
	for (auto it = clusters[cluster].get_elementos().begin();
		  it != clusters[cluster].get_elementos().end(); ++it){

		// buscamos la restricción asociada al elemento dado con todos los elementos
		// pertenecientes al cluster
		auto pos = restricciones.find(std::make_pair(elemento, (*it)));

		// si encuentra una restricción asociada y es que no pueden ir juntos,
		// quiere decir que no cumple dichas restricciones
		if (pos != restricciones.end() && (*pos).second == -1){
			incumplidas++;
		}

 	}




	// para todos los demas clusters
	for (unsigned i = 0; i < clusters.size(); i++){
		if ( (int)i != cluster){
			// buscamos si es obigatorio que tengan que ir juntos
			for (auto it = clusters[i].get_elementos().begin();
				  it != clusters[i].get_elementos().end(); ++it){

				auto pos = restricciones.find(std::make_pair(elemento, (*it)));
				// si tienen que ir juntos, no cumple las restricciones ya que estamos
				// intentando meterlo en un cluster distinto
				if (pos != restricciones.end() && (*pos).second == 1)
					incumplidas++;
			}
		}

	}


	return incumplidas;
}


void PAR::calcular_desviacion_general(){

	desviacion_general = 0.0d;

	// para todos los clusters
	for (unsigned i = 0; i < clusters.size(); i++){
		// calculamos su distancia intra cluster
		clusters[i].calcular_distancia_intra_cluster();

		// la sumamos a la desviacion general
		desviacion_general += clusters[i].get_distancia_intra_cluster();
	}

	// dividimos la desviación general por el total de clusters
	desviacion_general /= clusters.size();
}




std::pair<std::vector<PAR::Cluster>,int> PAR::algoritmo_BL(){

	generar_solucion_aleatoria();

	const double LAMBDA = mayor_distancia / (restricciones.size()/2.0);

	bool he_encontrado_mejor = false;

	int evaluaciones = 0;

	// inicialización de indices aleatorios
	std::vector<int> indices;
	std::vector<int> indices_clusters;

	for (unsigned i = 0; i < datos.size(); i++){
		indices.push_back(i);
	}


	for (unsigned i = 0; i < clusters.size(); i++){
		indices_clusters.push_back(i);
	}

	int n_infac = calcular_infactibilidad();
	int infac = n_infac;

	double f_objetivo = get_desviacion_general() + (n_infac*1.0d * LAMBDA);
	double n_f_objetivo = 0.0D;

	//std::vector<Cluster> sol = clusters;


	do {

		he_encontrado_mejor = false;

		// para explorar en vecindario de forma aleatoria
		std::random_shuffle(indices.begin(), indices.end(), RandPositiveInt);
		std::random_shuffle(indices_clusters.begin(), indices_clusters.end(), RandPositiveInt);


		// exploramos el vecindario
		for (auto it = indices.begin(); it != indices.end() && !he_encontrado_mejor; ++it){

			// para los distintos clusters
			for (auto it_c = indices_clusters.begin(); it_c != indices_clusters.end() && !he_encontrado_mejor; ++it_c){
				// solo cambiamos si el cluster es distinto al cluster donde se
				// encuetra anteriormente el elemento

				int antiguo = buscar_elemento( (*it) );

				if (antiguo != (*it_c) && clusters[antiguo].num_elementos() - 1 > 0){

					clusters[antiguo].delete_elemento( (*it) );

					n_infac -= cumple_restricciones( (*it), antiguo );
					n_infac += cumple_restricciones( (*it), (*it_c) );


					clusters[(*it_c)].add_elemento( (*it) );

					calcular_desviacion_general();

					n_f_objetivo = get_desviacion_general() + (n_infac * 1.0d * LAMBDA);

					evaluaciones++;

					if ( n_f_objetivo < f_objetivo ){

						f_objetivo = n_f_objetivo;
						infac = n_infac;
						//sol = clusters;
						he_encontrado_mejor = true;
					} else {
						//clusters = sol;
						clusters[(*it_c)].delete_elemento( (*it) );
						clusters[antiguo].add_elemento( (*it) );
						n_infac = infac;
					}

				}
			}
		}



		// si no tengo mejor vecino o llego al tope
	} while (he_encontrado_mejor && evaluaciones < TOPE_BL);

	//clusters = sol;

	calcular_desviacion_general();

	return std::make_pair(clusters, calcular_infactibilidad());


}






void PAR::generar_solucion_aleatoria(){

	for (auto it = clusters.begin(); it != clusters.end(); ++it){
		it->limpiar();
	}

	// inicialización de indices aleatorios
	std::vector<int> indices;

	for (unsigned i = 0; i < datos.size(); i++){
		indices.push_back(i);
	}

	std::random_shuffle(indices.begin(), indices.end(), RandPositiveInt);

	auto it = indices.begin();

	// metemos en los clusters al menos un elemento
	for (unsigned i = 0; i < clusters.size(); i++){
		clusters[i].add_elemento((*it));
		++it;
	}

	// introducimos los elementos restantes escogiendo los clusters de forma aleatoria

	while (it != indices.end()){
		clusters[RandPositiveInt(clusters.size())].add_elemento((*it));
		++it;
	}


	calcular_desviacion_general();

}

int PAR::calcular_infactibilidad() const{
	int infac = 0;



	for (auto it = restricciones.begin(); it != restricciones.end() ; ++it){
		// solo comprobamos si el primer elemento es mayor que el segundo
		// para no comprobar repetidas
		if ( (*it).first.first > (*it).first.second ){
			auto pos = restricciones.find( std::make_pair((*it).first.first, (*it).first.second) );

			if (pos != restricciones.end()){

				int c1 = buscar_elemento((*it).first.first);
				int c2 = buscar_elemento((*it).first.second);


				// si tienen que estar juntos y no lo estan
				if (pos->second == 1 && c1 != c2){
					infac++;
				}

				// si tienen que estar separados y no lo están
				if (pos->second == -1 && c1 == c2){
					infac++;
				}

			}

		}

	}

	return infac;
}


int PAR::buscar_elemento(const int elemento) const{
	int ret = -1;
	bool encontrado = false;;

	for (unsigned i = 0; i < clusters.size() && !encontrado; i++){
		auto it = clusters[i].get_elementos().find(elemento);

		if (it != clusters[i].get_elementos().end()){
			ret = i;
		}
	}

	return ret;
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

	for (auto it = centroide.begin(); it != centroide.end(); ++it){
		(*it) = 0.0d;
	}

	for(auto it = elementos.begin(); it != elementos.end(); ++it){
		for (unsigned j = 0; j < problema.datos[(*it)].size(); j++){
			centroide[j] += problema.datos[(*it)][j];
		}
	}

	for (unsigned i = 0; i < centroide.size(); i++){
		centroide[i] /= elementos.size()*1.0;
	}

}


void PAR::Cluster::calcular_distancia_intra_cluster(){
	auto it = elementos.begin();

	double distancia = 0;

	calcular_centroide();


	for (auto it = elementos.begin(); it != elementos.end(); ++it){
		distancia += problema.distancia_puntos(problema.datos[(*it)], centroide);
	}

	distancia /= num_elementos();

	distancia_intra_cluster = distancia;

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

double PAR::Cluster::get_distancia_intra_cluster() const{
	return distancia_intra_cluster;
}


unsigned PAR::Cluster::num_elementos() const{
	return elementos.size();
}



using namespace std;

ostream & operator << (ostream & flujo, const PAR::Cluster & clus) {

	// sacamos el centroide
	flujo << endl << "Centroide: " << endl;
	for (auto it = clus.get_centroide().begin(); it != clus.get_centroide().end(); ++it){
		flujo << (*it) << " ";
	}

	flujo << endl << endl;

	// sacamos la distancia intra cluster
	flujo << "Distancia intra-cluster: " << endl;
	flujo << clus.get_distancia_intra_cluster() << endl << endl;

	// mostramos los elementos
	flujo << "Elementos:" << endl;
	for (auto it = clus.get_elementos().begin(); it != clus.get_elementos().end(); ++it){
		flujo << (*it) << " ";
	}

	flujo << endl << endl;

	return flujo;
}



ostream & operator << (ostream & flujo, const PAR & par) {

	// sacamos la mayor distancia
	flujo << endl << "Mayor distancia: " << endl;
	flujo << par.mayor_distancia << endl;

	flujo << "Numero de restricciones: " << endl;
	flujo << par.restricciones.size()/2 << endl;

	flujo << endl;

	// sacamos la desviacion general
	flujo << "Desviacion general: " << endl;
	flujo << par.get_desviacion_general() << endl << endl;

	// sacamos la restricciones incumplidas
	flujo << "Restricciones inclumplidas: " << endl;
	flujo << par.calcular_infactibilidad() << endl << endl;


	// mostramos los clusters
	flujo << "Mostramos los clusters: " << endl;
	for (int i = 0; i < par.get_num_clusters(); i++){
		flujo << "Cluster " << i << endl;
		flujo << par.clusters[i];
	}

	flujo << endl;

	return flujo;
}
