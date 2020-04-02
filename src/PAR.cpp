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

	for (auto it = restricciones.begin(); it != restricciones.end(); ++it){
		lista_restricciones.push_back( std::make_tuple( (*it).first.first, (*it).first.second, (*it).second ) );
	}


	// creamos todos
	for (int i = 0; i < n_clusters; i++){
		clusters.push_back( Cluster((*this)) );
	}

	double d;

	mayor_distancia = 0;

	for (unsigned i = 0; i < datos.size(); i++){
		for (unsigned j = i; j < datos.size(); j++){
			d = distancia_puntos(datos[i], datos[j]);
			if (d > mayor_distancia){
				mayor_distancia = d * 1.0;
			}

		}
	}

	std::cout << "Mayor distancia " << mayor_distancia << std::endl;
	LAMBDA = get_mayor_distancia() / (restricciones.size()/2.0);


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


	return std::make_pair(clusters, calcular_infactibilidad());

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

	return cluster_menor_distancia;
}




/**
	@brief Función para calcular la distancia entre dos puntos

	@param p1 Punto 1
	@param p2 Punto 2

	@return double Distancia entre dos puntos
*/

double PAR::distancia_puntos(const std::vector<double> & p1,
									  const std::vector<double> & p2) const{


	double distancia = 0.0;

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




std::pair<std::vector<PAR::Cluster>,int> PAR::algoritmo_BL(const std::vector<Cluster> & ini){

	clusters = ini;

	calcular_desviacion_general();


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

	double f_objetivo = get_desviacion_general() + (n_infac*1.0d * get_lambda());
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

					n_f_objetivo = get_desviacion_general() + (n_infac * 1.0d * get_lambda());

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






std::vector<PAR::Cluster> PAR::generar_solucion_aleatoria(){

	std::vector<PAR::Cluster> sol;

	for (int i = 0; i < clusters.size(); i++){
			sol.push_back( Cluster((*this)) );
		sol[i].limpiar();
	}


	// inicialización de indices aleatorios
	std::vector<int> indices;

	for (unsigned i = 0; i < datos.size(); i++){
		indices.push_back(i);
	}

	std::random_shuffle(indices.begin(), indices.end(), RandPositiveInt);

	auto it = indices.begin();

	// metemos en los clusters al menos un elemento
	for (unsigned i = 0; i < sol.size(); i++){
		sol[i].add_elemento((*it));
		++it;
	}

	// introducimos los elementos restantes escogiendo los clusters de forma aleatoria

	while (it != indices.end()){
		sol[RandPositiveInt(sol.size())].add_elemento((*it));
		++it;
	}

	return sol;

}

int PAR::calcular_infactibilidad() const{
	int infac = 0;


	/*
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

	}*/

	for (auto it = lista_restricciones.begin(); it != lista_restricciones.end(); ++it){
		if ( std::get<0>((*it)) > std::get<1>((*it)) ){
			int c1 = buscar_elemento(std::get<0>((*it)));
			int c2 = buscar_elemento(std::get<1>((*it)));


			// si tienen que estar juntos y no lo estan
			if (std::get<2>((*it)) == 1 && c1 != c2){
				infac++;
			}

			// si tienen que estar separados y no lo están
			if (std::get<2>((*it)) == -1 && c1 == c2){
				infac++;
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


double PAR::get_mayor_distancia() const{
	return mayor_distancia;
}


double PAR::get_lambda() const{
	return LAMBDA;
}


/*

PRACTICA 2


*/




std::pair<std::vector<PAR::Cluster>, int> PAR::algoritmo_AGG(const int evaluaciones_max, const int tam_pob_ini, const float prob_mutacion, const float prob_cruce){
	std::vector<std::vector<int>> poblacion = generar_poblacion_inicial(tam_pob_ini);

	std::vector<std::vector<int>> poblacion_intermedia;

	int i = 0;

	while (i < evaluaciones_max){

		poblacion_intermedia = seleccion_AG(poblacion);


		poblacion_intermedia = operador_cruce_uniforme_AGG(poblacion_intermedia, prob_cruce);



	}

}



// operador de seleccion para el algoritmo generacional
std::vector<std::vector<int>> PAR::seleccion_AGG(const std::vector<std::vector<int>> & poblacion){

	std::vector<std::vector<int>> poblacion_intermedia;


	while (poblacion_intermedia.size() != poblacion.size()){
		double valoracion_c1, valoracion_c2;
		int primer_candidato = RandPositiveInt(poblacion.size());
		int segundo_candidato;

		do {
			segundo_candidato = RandPositiveInt(poblacion.size());
		} while (segundo_candidato == primer_candidato);

		clusters = solucion_to_clusters(poblacion[primer_candidato]);
		calcular_desviacion_general();
		valoracion_c1 = get_desviacion_general() + (calcular_infactibilidad() * get_lambda());

		clusters = solucion_to_clusters(poblacion[segundo_candidato]);
		calcular_desviacion_general();
		valoracion_c2 = get_desviacion_general() + (calcular_infactibilidad() * get_lambda());

		if (valoracion_c1 > valoracion_c2){
			poblacion_intermedia.push_back(poblacion[primer_candidato]);
		} else {
			poblacion_intermedia.push_back(poblacion[segundo_candidato]);
		}

	}

	return poblacion_intermedia;

}

std::vector<std::vector<int>> PAR::operador_cruce_uniforme(const std::vector<std::vector<int>> & poblacion, const double prob_cruce){

	const int NUM_PAREJAS = poblacion.size()/2;
	const int NUM_CRUCES = NUM_PAREJAS * prob_cruce;

	std::vector<int> valores;
	std::vector<int> valores2;

	std::vector<std::vector<int>> poblacion_intermedia;
	std::vector<int> cruze;

	// cruzamos los elementos, no importa el orden porque ya se desordenaron en la
	// selección, por lo que cruzamos los NUM_CRUCES primeros, el i con el i+1 y ya
	// por eso vamos hasta NUM_CRUCES*2, y  sumamos +2 cada iteracion
	// por  cada cruce tenemos que generar dos hijos, así que ejecutamos NUM_CRUCES*2
	int indice = 0;
	for (int i = 0; i < NUM_CRUCES*2; i++ ){

		if (i % 2 == 0){
			// si i es par es el primer hijo
			indice = i;
		} else {
			// si i es impar es el segundo hijo, el indice es uno menos
			indice = i - 1;
		}

		valores.clear();

		while (valores.size() < poblacion[indice].size()/2){
			int valor = RandPositiveInt(poblacion[indice].size());

			auto pos = valores.find(valor);
			if (pos == valores.end()){
				valores.push_back(valor);
			}
		}

		cruze1.resize(poblacion[indice].size());

		for (int j = 0; < poblacion[j].size(); j++){
			auto pos = valores.find(j);

			// rellenamos el hijo 1
			// esta entre los seleccionados aleatoriamente, lo cogemos del padre 1
			if (pos != valores.end()){
				cruze[j] = poblacion[indice][j];
			} else {
				// si no está lo cogemos del padre 2
				cruze[j] = poblacion[indice+1][j];
			}
		}

		reparar_cruce(cruce);

		poblacion_intermedia.push_back(cruce);

	}


	return poblacion_intermedia;

}






std::vector<std::vector<int>> PAR::operador_cruce_seg_fijo(const std::vector<std::vector<int>> & poblacion, const double prob_cruce){

	const int NUM_PAREJAS = poblacion.size()/2;
	const int NUM_CRUCES = NUM_PAREJAS * prob_cruce;

	std::vector<int> valores;
	std::vector<int> valores2;

	std::vector<std::vector<int>> poblacion_intermedia;
	std::vector<int> cruze;

	int tam_segmento;
	int ini_segmento;

	// cruzamos los elementos, no importa el orden porque ya se desordenaron en la
	// selección, por lo que cruzamos los NUM_CRUCES primeros, el i con el i+1 y ya
	// por eso vamos hasta NUM_CRUCES*2, y  sumamos +2 cada iteracion
	// por  cada cruce tenemos que generar dos hijos, así que ejecutamos NUM_CRUCES*2
	int indice = 0;
	for (int i = 0; i < NUM_CRUCES*2; i++ ){
		if (i % 2 == 0){
			// si i es par es el primer hijo
			indice = i;
		} else {
			// si i es impar es el segundo hijo, el indice es uno menos
			indice = i - 1;
		}

		// generamos el tamaño de segmento aleatorio
		tam_segmento = RandPositiveInt(poblacion[i].size());
		ini_segmento = RandPositiveInt(poblacion[i].size());

		cruze = std::vector<int>(poblacion[i].size(), -1);

		int j = ini_segmento;
		// copiamos del padre 1 el segmento [ini_segmento, ini_segmento+tam_segmento]
		while (j <= ini_segmento+tam_segmento){
			cruce[j%poblacion[indice].size()] = poblacion[indice][j%poblacion[indice].size];
			j++;
		}

		// copiamos del padre 2 [ini_segmento+tam_segmento, fin], con modulos por si se pasa del final
		while(j < poblacion[indice+1].size()){
			cruce[j%poblacion[indice+1].size()] = poblacion[indice+1][j%poblacion[indice+1].size];
			j++;
		}

		reparar_cruce(cruce);

		poblacion_intermedia.push_back(cruce);


	}

	return poblacion_intermedia;

}


void PAR::reparar_cruce(std::vector<int> & reparado){

	std::vector<int> contador(clusters.size(), 0);

	for (auto it = reparado.begin(); it != reparado.end(); ++it){
		contador[(*it)]++;
	}


	for (int i = 0; i < contador.size(); i++){
		// si tengo un cluster vacio
		if (contador[i] == 0){
			// de todos los elementos, saco uno aleatorio
			int elemento_aleatorio;
			do {
				elemento_aleatorio = RandPositiveInt(reparado.size());
				// hasta que encuentre un cluster que quitando un elemento no se quede vacio
			} while (contador[reparado[elemento_aleatorio]] - 1 > 0 );

			contador[reparado[elemento_aleatorio]]--;
			reparado[elemento_aleatorio] = i;
			contador[i]++;

		}
	}

	return reparado;

}




void PAR::operador_mutacion_uniforme(std::vector<std::vector<int>> & poblacion , const double prob_mut){

	const int NUM_MUTACIONES = poblacion.size()*poblacion[0].size() * prob_mut;

	std::vector<int> contador (clusters.size(), 0);

	int elemento_poblacion;
	int gen;
	int destino;

	for (int i = 0; i < NUM_MUTACIONES; i++){
		elemento_poblacion = RandPositiveInt(poblacion.size());
		contador = std::vector<int>(clusters.size(), 0);

		for (auto it = poblacion[elemento].begin(); it != poblacion[elemento].end(); ++it){
			contador[(*it)]++;
		}

		do {
			gen = RandPositiveInt(poblacion[elemento_poblacion].size());

			// mientras que el tam del cluster del elemento inicial - 1 sea > 0
			// es decir, que si movemos ese elemento no se quede el cluster vacio
		} while (contador[poblacion[elemento_poblacion][gen]] - 1 > 0);


		destino = RandPositiveInt(clusters.size());

		contador[poblacion[elemento_poblacion][gen]]--;
		poblacion[elemento_poblacion][gen] = destino;
		contador[destino]++;

	}

}








std::vector<std::vector<int>> PAR::generar_poblacion_inicial(const int tam_pob_ini){
	std::vector<std::vector<int>> poblacion;

	for (int i = 0; i < tam_pob_ini; i++){
		// generamos una población aleatoria, la pasamos a la representacion usada
		// en los algoritmos geneticos, y la introducimos en la matriz de poblacion
		poblacion.push_back(clusters_to_solucion(generar_solucion_aleatoria()));
	}

	return poblacion;
}



std::vector<int> PAR::clusters_to_solucion(const std::vector<PAR::Cluster> & clusters_ini) const {

	std::vector<int> solucion;

	int tam_total = 0;

	for (unsigned i = 0; i < clusters_ini.size(); i++){
		tam_total += clusters_ini[i].num_elementos();
	}

	solucion.resize(tam_total);

	for (unsigned i = 0; i < clusters_ini.size(); i++){
		for (auto it = clusters_ini[i].get_elementos().begin();
		     it != clusters_ini[i].get_elementos().end(); ++it ){
			solucion[(*it)] = i;
		}
	}

	return solucion;
}


std::vector<PAR::Cluster> PAR::solucion_to_clusters(const std::vector<int> & sol) const{

	std::vector<PAR::Cluster> devolver;

	for (int i = 0; i < clusters.size(); i++){
		devolver.push_back( Cluster((*this)) );
	}

	for (int i = 0; i < sol.size(); i++){
		devolver[sol[i]].add_elemento(i);
	}

	return devolver;

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

	int infact = par.calcular_infactibilidad();
	flujo << "Datos para las tablas, ordenados:" << endl;
	flujo << par.get_desviacion_general() << "\t" << infact << "\t" << par.get_desviacion_general() + (infact * (par.get_mayor_distancia()/(par.restricciones.size()/2))) << endl;
	// sacamos la mayor distancia
	flujo << endl << "Mayor distancia: " << endl;
	flujo << par.get_mayor_distancia() << endl;

	flujo << "Numero de restricciones: " << endl;
	flujo << par.restricciones.size()/2 << endl;

	flujo << endl;

	// sacamos la desviacion general
	flujo << "Desviacion general: " << endl;
	flujo << par.get_desviacion_general() << endl << endl;

	// sacamos la restricciones incumplidas
	flujo << "Restricciones inclumplidas: " << endl;
	flujo << infact << endl << endl;


	// mostramos los clusters
	flujo << "Mostramos los clusters: " << endl;
	for (int i = 0; i < par.get_num_clusters(); i++){
		flujo << "Cluster " << i << endl;
		flujo << par.clusters[i];
	}

	flujo << endl;

	return flujo;
}
