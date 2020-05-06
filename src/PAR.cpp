#include "PAR.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <ctime>
#include "random.h"
#include <cmath>




/*

Clase PAR

*/

PAR::PAR(const std::string fichero_datos, const std::string fichero_restricciones,
	 		const int n_clusters, const std::string semilla):NOM_DATOS(fichero_datos), NOM_RESTRICCIONES(fichero_restricciones)
			, SEMILLA(semilla){

	leer_datos(fichero_datos);
	leer_restricciones(fichero_restricciones);

	for (auto it = restricciones.begin(); it != restricciones.end(); ++it){
		lista_restricciones.push_back( std::make_tuple( (*it).first.first, (*it).first.second, (*it).second ) );
		if ( (*it).first.first > (*it).first.second ){
			lista_restricciones_superior.push_back(std::make_tuple( (*it).first.first, (*it).first.second, (*it).second ));
		}
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

	//std::cout << "Mayor distancia " << mayor_distancia << std::endl;
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



std::pair<std::vector<PAR::Cluster>,double> PAR::algoritmo_greedy(){


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
	std::vector<bool> cambios_anterior = cambios;

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

		hay_cambios = cambios != cambios_anterior;
		cambios_anterior = cambios;

	} while(hay_cambios);


	clusters = n_sol;

	calcular_desviacion_general();


	return std::make_pair(clusters, funcion_objetivo());

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




std::pair<std::vector<PAR::Cluster>,double> PAR::algoritmo_BL(const std::vector<Cluster> & ini, int & TOPE_BL, const bool salida){



	std::string fichero = "graficas/";

	fichero += NOM_RESTRICCIONES;
	fichero += "_" + SEMILLA;
	fichero += "_BL.out";

	std::fstream fic;
	if (salida){
		fic.open (fichero, std::fstream::out);
	}

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

					if (salida){
						fic << evaluaciones << "\t" << f_objetivo << std::endl;
					}
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

	// establecemos TOPE_BL a las evaluacinoes realizadas, para devolverlas por parametro
	TOPE_BL = evaluaciones;

	calcular_desviacion_general();

	return std::make_pair(clusters, funcion_objetivo());


}






std::vector<PAR::Cluster> PAR::generar_solucion_aleatoria(){

	std::vector<PAR::Cluster> sol;

	for (unsigned i = 0; i < clusters.size(); i++){
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

	for (auto it = lista_restricciones_superior.begin(); it != lista_restricciones_superior.end(); ++it){
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
	bool encontrado = false;

	for (unsigned i = 0; i < clusters.size() && !encontrado; i++){
		auto it = clusters[i].get_elementos().find(elemento);

		if (it != clusters[i].get_elementos().end()){
			ret = i;
			encontrado = true;
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




std::pair<std::vector<PAR::Cluster>, double> PAR::algoritmos_AG(const unsigned evaluaciones_max,
																			 	 	 const unsigned tam_pob_ini,
																				  	 const float prob_mutacion,
																				 	 const float prob_cruce,
																				 	 const operador_cruce tipo_cruce,
																				 	 const tipo_generacion tipo_generaciones,
																			 	 	 const bool elitismo){



	std::string fichero = "graficas/";

	fichero += NOM_RESTRICCIONES;
	fichero += "_" + SEMILLA;

	int evaluaciones_BL_normal = 100000;

	if (tipo_generaciones == tipo_generacion::GENERACIONAL){
		fichero += "_AGG";
	} else if (tipo_generaciones == tipo_generacion::ESTACIONARIO){
		fichero += "_AGE";
	} else if (tipo_generaciones == tipo_generacion::MEMETICO_1){
		fichero += "_AM-1";
	} else if (tipo_generaciones == tipo_generacion::MEMETICO_0_1){
		fichero += "_AM-0_1";
	} else if (tipo_generaciones == tipo_generacion::MEMETICO_0_1_MEJ){
		fichero += "_AM-0_1mej";
	}  else if (tipo_generaciones == tipo_generacion::MEMETICO_BL_1){
		fichero += "_AM-BL-1";
	} else if (tipo_generaciones == tipo_generacion::MEMETICO_BL_0_1){
		fichero += "_AM-BL-0_1";
	} else if (tipo_generaciones == tipo_generacion::MEMETICO_BL_0_1_MEJ){
		fichero += "_AM-BL-0_1mej";
	}

	if (tipo_cruce == operador_cruce::SEGMENTO_FIJO){
		fichero += "_SF.out";
	} else {
		fichero += "_UN.out";
	}

	std::fstream fic;
	fic.open (fichero, std::fstream::out);
	//fic << std::endl << std::endl;

	std::vector<std::vector<int>> p = generar_poblacion_inicial(tam_pob_ini);

	std::vector<std::pair<std::vector<int>, double>> poblacion;
	std::vector<std::pair<std::vector<int>, double>> poblacion_anterior;


	poblacion.resize(p.size());

	for (unsigned i = 0; i < poblacion.size(); i++){
		poblacion[i].first = p[i];
		clusters = solucion_to_clusters(poblacion[i].first);
		calcular_desviacion_general();
		poblacion[i].second = get_desviacion_general() + (calcular_infactibilidad() * get_lambda());
	}

	poblacion_anterior = poblacion;
	std::pair<std::vector<int>, double> mejor = poblacion[0];

	int indice_peor = 0;

	for (unsigned i = 0; i < poblacion.size(); i++){
		if (poblacion[i].second < mejor.second){
			mejor = poblacion[i];
		}
	}

	unsigned generacion = 0;
	unsigned evaluaciones = 0;

	while (evaluaciones < evaluaciones_max){

		if (tipo_generaciones == tipo_generacion::ESTACIONARIO){
			poblacion = seleccion_algoritmos_ev(poblacion_anterior, 2);
		} else {
			// generacional por defecto
			poblacion = seleccion_algoritmos_ev(poblacion_anterior, poblacion_anterior.size());
		}



		if (tipo_cruce == operador_cruce::SEGMENTO_FIJO){
			evaluaciones += operador_cruce_seg_fijo(poblacion, prob_cruce);
		} else {
			evaluaciones += operador_cruce_uniforme(poblacion, prob_cruce);
		}

		//std::cout << evaluaciones << std::endl;

		evaluaciones += operador_mutacion_uniforme(poblacion, prob_mutacion, tipo_generaciones);

		//std::cout << evaluaciones << std::endl;


		// reemplazamiento de la poblacion

		if (tipo_generaciones != tipo_generacion::ESTACIONARIO){
			if (elitismo){
				// miramos si el mejor de la anterior sigue estando
				auto pos_mejor = find(poblacion.begin(), poblacion.end(), mejor);

				// si no esta lo añadimos
				if (pos_mejor == poblacion.end()){
					poblacion.push_back(mejor);

					// buscamos el peor de la nueva
					for (unsigned i = 0; i < poblacion.size(); i++){

						if (poblacion[i].second > poblacion[indice_peor].second){
							indice_peor = i;
						}
					}

					// y lo eliminamos
					auto it = poblacion.begin();
					std::advance(it, indice_peor);
					poblacion.erase(it);
				}

			}

			poblacion_anterior = poblacion;

		} else {
			int indice_peor = 0, indice_segundo_peor = 1;

			if (poblacion[0].second > poblacion[1].second){
				poblacion.push_back(poblacion[0]);
				poblacion.erase(poblacion.begin());
			}

			for (unsigned i = 0; i < poblacion_anterior.size(); i++){
				if (poblacion_anterior[i].second > poblacion_anterior[indice_peor].second){
					indice_peor = i;
				}

				if (poblacion_anterior[i].second > poblacion_anterior[indice_segundo_peor].second &&
					 indice_segundo_peor != indice_peor){
					indice_segundo_peor = i;
				}
			}


			for (unsigned i = 0; i < poblacion.size(); i++){
				if (indice_peor != -1 && poblacion[i].second < poblacion_anterior[indice_peor].second){
					auto it = poblacion_anterior.begin();
					std::advance(it, indice_peor);

					poblacion_anterior.erase(it);
					poblacion_anterior.push_back(poblacion[i]);

					indice_peor = -1;
				} else if (indice_segundo_peor != -1 && poblacion[i].second < poblacion_anterior[indice_segundo_peor].second){
					auto it = poblacion_anterior.begin();
					std::advance(it, indice_segundo_peor);

					poblacion_anterior.erase(it);
					poblacion_anterior.push_back(poblacion[i]);

					indice_segundo_peor = -1;
				}

			}




		}

		if (generacion % poblacion_anterior.size() == 0){
			evaluaciones_BL_normal = 100000;
			if (tipo_generaciones == tipo_generacion::MEMETICO_1 || tipo_generaciones == tipo_generacion::MEMETICO_BL_1 ){
				if (tipo_generaciones == tipo_generacion::MEMETICO_1){
					for (unsigned i = 0; i < poblacion_anterior.size(); i++){
						evaluaciones += algoritmo_BL_suave(poblacion_anterior[i], 0.1*poblacion_anterior[i].first.size());
					}
				} else {
					for (unsigned i = 0; i < poblacion_anterior.size(); i++){
						auto sol_BL = algoritmo_BL(solucion_to_clusters(poblacion_anterior[i].first), evaluaciones_BL_normal);
						poblacion_anterior[i].first = clusters_to_solucion(sol_BL.first);
						poblacion_anterior[i].second = sol_BL.second;
						evaluaciones += evaluaciones_BL_normal;
					}
				}

			} else if (tipo_generaciones == tipo_generacion::MEMETICO_0_1 || tipo_generaciones == tipo_generacion::MEMETICO_BL_0_1){
				// volvemos a aplicar la esperanza matemática
				const unsigned NUM_BL = 0.1 * poblacion_anterior.size();

				if (tipo_generaciones == tipo_generacion::MEMETICO_0_1){
					for (unsigned i = 0; i < NUM_BL; i++){
						evaluaciones += algoritmo_BL_suave(poblacion_anterior[i], 0.1*poblacion_anterior[i].first.size());
					}
				} else {
					for (unsigned i = 0; i < NUM_BL; i++){
						auto sol_BL = algoritmo_BL(solucion_to_clusters(poblacion_anterior[i].first), evaluaciones_BL_normal);
						poblacion_anterior[i].first = clusters_to_solucion(sol_BL.first);
						poblacion_anterior[i].second = sol_BL.second;
						evaluaciones += evaluaciones_BL_normal;
					}
				}

			} else if (tipo_generaciones == tipo_generacion::MEMETICO_0_1_MEJ || tipo_generaciones == tipo_generacion::MEMETICO_BL_0_1_MEJ){
				const unsigned NUM_MEJORES = 0.1 * poblacion_anterior.size();
				std::set<int> indices_mejores;
				for (unsigned i = 0; i < NUM_MEJORES; i++){
					int indice_mejor = 0;
					for (unsigned j = 0; j < poblacion_anterior.size(); j++){
						if (poblacion_anterior[indice_mejor].second > poblacion_anterior[j].second &&
							 indices_mejores.find(j) == indices_mejores.end() ){
							indice_mejor = j;
						}
					}
					indices_mejores.insert(indice_mejor);
				}


				if (tipo_generaciones == tipo_generacion::MEMETICO_0_1_MEJ){
					for (auto it = indices_mejores.begin(); it != indices_mejores.end(); ++it){
						evaluaciones += algoritmo_BL_suave(poblacion_anterior[(*it)], 0.1*poblacion_anterior[(*it)].first.size());
					}
				} else {
					for (auto it = indices_mejores.begin(); it != indices_mejores.end(); ++it){
						auto sol_BL = algoritmo_BL(solucion_to_clusters(poblacion_anterior[(*it)].first), evaluaciones_BL_normal);
						poblacion_anterior[(*it)].first = clusters_to_solucion(sol_BL.first);
						poblacion_anterior[(*it)].second = sol_BL.second;
						evaluaciones += evaluaciones_BL_normal;
					}
				}


			}
		}

		for (unsigned i = 0; i < poblacion_anterior.size(); i++){
			if (poblacion_anterior[i].second < mejor.second){
				mejor = poblacion_anterior[i];
			}
		}

		fic << generacion << "\t" << mejor.second << std::endl;

		generacion++;
	}

	clusters = solucion_to_clusters(mejor.first);
	calcular_desviacion_general();

	return std::make_pair(clusters, funcion_objetivo());

}



// operador de seleccion para el algoritmo generacional
std::vector<std::pair<std::vector<int>, double>> PAR::seleccion_algoritmos_ev(const std::vector<std::pair<std::vector<int>, double>> & poblacion, const int tam){

	std::vector<std::pair<std::vector<int>, double>> poblacion_intermedia;


	while ((int)poblacion_intermedia.size() != tam){
		int primer_candidato = RandPositiveInt(poblacion.size());
		int segundo_candidato;

		do {
			segundo_candidato = RandPositiveInt(poblacion.size());
		} while (segundo_candidato == primer_candidato);


		if (poblacion[primer_candidato].second > poblacion[segundo_candidato].second){
			poblacion_intermedia.push_back(poblacion[segundo_candidato]);
		} else {
			poblacion_intermedia.push_back(poblacion[primer_candidato]);
		}

	}

	return poblacion_intermedia;

}

unsigned PAR::operador_cruce_uniforme(std::vector<std::pair<std::vector<int>, double>> & poblacion, const double prob_cruce){

	const int NUM_PAREJAS = poblacion.size()/2;
	const int NUM_CRUCES = NUM_PAREJAS * prob_cruce;

	unsigned evaluaciones = 0;

	std::set<int> valores;

	//std::vector<std::pair<std::vector<int>, double>> poblacion_intermedia;
	std::vector<int> cruce;

	// cruzamos los elementos, no importa el orden porque ya se desordenaron en la
	// selección, por lo que cruzamos los NUM_CRUCES primeros, el i con el i+1 y ya
	// por eso vamos hasta NUM_CRUCES*2, y  sumamos +2 cada iteracion
	// por  cada cruce tenemos que generar dos hijos, así que ejecutamos NUM_CRUCES*2
	int indice = 0;
	for (int i = 0; i < NUM_CRUCES*2; i++ ){

		/* el indice se mantiene a cero, vamos a ir borrando
		if (i % 2 == 0){
			// si i es par es el primer hijo
			indice = i;
		} else {
			// si i es impar es el segundo hijo, el indice es uno menos
			indice = i - 1;
		}
		*/

		valores.clear();

		while (valores.size() < poblacion[indice].first.size()/2){
			int valor = RandPositiveInt(poblacion[indice].first.size());

			auto pos = valores.find(valor);//std::find(valores.begin(), valores.end(), valor);
			if (pos == valores.end()){
				valores.insert(valor);
			}
		}

		cruce.resize(poblacion[indice].first.size());

		for (unsigned j = 0; j < poblacion[indice].first.size(); j++){
			auto pos = valores.find(j);

			// rellenamos el hijo 1
			// esta entre los seleccionados aleatoriamente, lo cogemos del padre 1
			if (pos != valores.end()){
				cruce[j] = poblacion[indice].first[j];
			} else {
				// si no está lo cogemos del padre 2
				cruce[j] = poblacion[indice+1].first[j];
			}
		}

		reparar_cruce(cruce);

		clusters = solucion_to_clusters(cruce);
		calcular_desviacion_general();

		std::pair<std::vector<int>, double> cruzado (cruce, funcion_objetivo());
		evaluaciones++;

		poblacion.push_back(cruzado);

		// hemos hecho los dos cruces, el de padre 0 y el de padre 1
		if (i % 2 == 1){
			// borramos los dos primeros
			poblacion.erase(poblacion.begin());
			poblacion.erase(poblacion.begin());
		}

	}

	/* ya no hace falta copiarlos, no los hemos borrado
	// copiamos resto de poblacion no cruzada
	for (unsigned i = NUM_CRUCES*2; i < poblacion.size(); i++){
		poblacion_intermedia.push_back(poblacion[i]);
	}*/


	//return poblacion_intermedia;
	return evaluaciones;

}






unsigned PAR::operador_cruce_seg_fijo(std::vector<std::pair<std::vector<int>, double>> & poblacion, const double prob_cruce){

	const int NUM_PAREJAS = poblacion.size()/2;
	const int NUM_CRUCES = NUM_PAREJAS * prob_cruce;

	std::set<int> valores;

	unsigned evaluaciones = 0;

	//std::vector<std::pair<std::vector<int>, double>> poblacion_intermedia;
	std::vector<int> cruce;

	int tam_segmento;
	int ini_segmento;
	int fin_segmento;

	int rango_fijo_low, rango_fijo_hight;

	// cruzamos los elementos, no importa el orden porque ya se desordenaron en la
	// selección, por lo que cruzamos los NUM_CRUCES primeros, el i con el i+1 y ya
	// por eso vamos hasta NUM_CRUCES*2, y  sumamos +2 cada iteracion
	// por  cada cruce tenemos que generar dos hijos, así que ejecutamos NUM_CRUCES*2
	int indice_p1 = 0;
	int indice_p2 = 1;
	for (int i = 0; i < NUM_CRUCES*2; i++ ){

		indice_p1 = 0;
		indice_p2 = 1;

		if (poblacion[indice_p2].second < poblacion[indice_p1].second){
			indice_p1 = 1;
			indice_p2 = 0;
		}

		/* el indice se mantiene siempre a 0, vamos a ir eliminando de la poblacion
		if (i % 2 == 0){
			// si i es par es el primer hijo
			indice = i;
		} else {
			// si i es impar es el segundo hijo, el indice es uno menos
			indice = i - 1;
		}*/

		// generamos el tamaño de segmento aleatorio
		tam_segmento = RandPositiveInt(poblacion[i].first.size());
		ini_segmento = RandPositiveInt(poblacion[i].first.size());

		fin_segmento = ini_segmento + tam_segmento + 1;

		cruce = std::vector<int>(poblacion[i].first.size(), -1);

		int j = ini_segmento;
		// copiamos del padre 1 el segmento [ini_segmento, ini_segmento+tam_segmento]
		while (j <= ini_segmento+tam_segmento){
			cruce[j%poblacion[indice_p1].first.size()] = poblacion[indice_p1].first[j%poblacion[indice_p1].first.size()];
			j++;
		}


		if (fin_segmento >= (int) poblacion[indice_p1].first.size()){

			rango_fijo_low = (fin_segmento % poblacion[indice_p1].first.size());
			rango_fijo_hight = ini_segmento;

		} else {

			rango_fijo_low = fin_segmento;
			rango_fijo_hight = poblacion[indice_p1].first.size() + ini_segmento;
		}

		valores.clear();

		while (valores.size() < (unsigned) (rango_fijo_hight-rango_fijo_low+1)/2){
			int valor = Randint(rango_fijo_low, rango_fijo_hight);

			auto pos = valores.find(valor);
			if (pos == valores.end()){
				valores.insert(valor % poblacion[indice_p1].first.size());
			}
		}

		for (int j = rango_fijo_low; j < rango_fijo_hight; j++){
			int v = j  % poblacion[indice_p1].first.size();
			auto pos = valores.find(v );//std::find(valores.begin(), valores.end(), j);

			// rellenamos el hijo 1
			// esta entre los seleccionados aleatoriamente, lo cogemos del padre 1
			if (pos != valores.end()){
				cruce[v] = poblacion[indice_p1].first[v];
			} else {
				// si no está lo cogemos del padre 2
				cruce[v] = poblacion[indice_p2].first[v];
			}
		}


		reparar_cruce(cruce);

		clusters = solucion_to_clusters(cruce);
		calcular_desviacion_general();

		std::pair<std::vector<int>, double> cruzado (cruce, funcion_objetivo());
		evaluaciones++;

		poblacion.push_back(cruzado);

		// hemos hecho los dos cruces, el de padre 0 y el de padre 1
		if (i % 2 == 1){
			// borramos los dos primeros
			poblacion.erase(poblacion.begin());
			poblacion.erase(poblacion.begin());
		}

	}

	/*
	// copiamos resto de poblacion no cruzada
	for (unsigned i = NUM_CRUCES*2; i < poblacion.size(); i++){
		poblacion_intermedia.push_back(poblacion[i]);
	}*/

	//return poblacion_intermedia;
	return evaluaciones;

}


void PAR::reparar_cruce(std::vector<int> & reparado){

	std::vector<int> contador(clusters.size(), 0);

	for (auto it = reparado.begin(); it != reparado.end(); ++it){
		contador[(*it)]++;
	}


	for (unsigned i = 0; i < contador.size(); i++){
		// si tengo un cluster vacio
		if (contador[i] == 0){
			// de todos los elementos, saco uno aleatorio
			int elemento_aleatorio;
			do {
				elemento_aleatorio = RandPositiveInt(reparado.size());
				// hasta que encuentre un cluster que quitando un elemento no se quede vacio
			} while (contador[reparado[elemento_aleatorio]] - 1 == 0 );

			contador[reparado[elemento_aleatorio]]--;
			reparado[elemento_aleatorio] = i;
			contador[i]++;

		}
	}


}




unsigned PAR::operador_mutacion_uniforme(std::vector<std::pair<std::vector<int>, double>> & poblacion ,
	 												  const double prob_mut,
												  	  const tipo_generacion tipo_g){


	std::vector<int> contador (clusters.size(), 0);

	unsigned evaluaciones = 0;

	int elemento_poblacion;
	int gen;
	int destino;
	int mut = 0;

	// si es en estacionario, NUM_MUTACIONES = 0, y vamos a probar a lanzarlo con un aleatorio
	// sugerencia por oscar en el foro de prado
	if (tipo_g == tipo_generacion::ESTACIONARIO){
		for (unsigned i = 0; i < poblacion.size(); i++){
			float aleatorio = Rand();

			if (aleatorio < prob_mut * poblacion[0].first.size()){
				mut++;
			}
		}

	} else {
		mut = poblacion.size()*poblacion[0].first.size() * prob_mut;
	}

	const int NUM_MUTACIONES = mut;



	for (int i = 0; i < NUM_MUTACIONES; i++){
		elemento_poblacion = RandPositiveInt(poblacion.size());
		contador = std::vector<int>(clusters.size(), 0);

		for (auto it = poblacion[elemento_poblacion].first.begin(); it != poblacion[elemento_poblacion].first.end(); ++it){
			contador[(*it)]++;
		}


		do {
			gen = RandPositiveInt(poblacion[elemento_poblacion].first.size());

			//std::cout << gen << " " << contador[poblacion[elemento_poblacion].first[gen]] << std::endl << std::flush;

			// mientras que el tam del cluster del elemento inicial - 1 sea <= 0
			// es decir, que si movemos ese elemento no se quede el cluster vacio
		} while (contador[poblacion[elemento_poblacion].first[gen]] - 1 <= 0);

		destino = RandPositiveInt(clusters.size());

		contador[poblacion[elemento_poblacion].first[gen]]--;
		poblacion[elemento_poblacion].first[gen] = destino;
		contador[destino]++;

		clusters = solucion_to_clusters(poblacion[elemento_poblacion].first);
		calcular_desviacion_general();
		poblacion[elemento_poblacion].second = funcion_objetivo();
		evaluaciones++;

	}

	return evaluaciones;

}


std::vector<std::vector<int>> PAR::generar_poblacion_inicial(const unsigned tam_pob_ini){
	std::vector<std::vector<int>> poblacion;

	/*
	for (int i = 0; i < tam_pob_ini/5; i++){
		// generamos una población aleatoria, la pasamos a la representacion usada
		// en los algoritmos geneticos, y la introducimos en la matriz de poblacion
		poblacion.push_back(clusters_to_solucion(algoritmo_BL(generar_solucion_aleatoria()).first));
	}*/

	while (poblacion.size() < tam_pob_ini){
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


std::vector<PAR::Cluster> PAR::solucion_to_clusters(const std::vector<int> & sol){

	std::vector<PAR::Cluster> devolver;

	for (unsigned i = 0; i < clusters.size(); i++){
		devolver.push_back( Cluster((*this)) );
	}

	for (unsigned i = 0; i < sol.size(); i++){
		devolver[sol[i]].add_elemento(i);
	}

	return devolver;

}



int PAR::algoritmo_BL_suave(std::pair<std::vector<int>, double> & sol_ini,
	 								 const unsigned fallos_permitidos){

	std::vector<int> indices;
	std::vector<int> contador(clusters.size(), 0);

	for (unsigned i = 0; i < sol_ini.first.size(); i++){
		indices.push_back(i);
		contador[sol_ini.first[i]]++;
	}

	std::random_shuffle(indices.begin(), indices.end(), RandPositiveInt);

	unsigned fallos = 0;
	bool mejora = true;
	unsigned i = 0;
	unsigned evaluaciones = 0;

	clusters = solucion_to_clusters(sol_ini.first);
	calcular_desviacion_general();

	std::pair<std::vector<int>, double> sol_intermedia = sol_ini;

	int mejor_cluster = -1;
	double val_mejor_cluster = sol_ini.second;

	while ( (mejora || fallos < fallos_permitidos) && i < sol_ini.first.size()){
		mejora = false;
		mejor_cluster = -1;

		val_mejor_cluster = sol_ini.second;
		sol_intermedia = sol_ini;

		for (int j = 0; j < get_num_clusters(); j++){
			if (sol_ini.first[indices[i]] != j && contador[sol_ini.first[indices[i]]] - 1 > 0){
				sol_intermedia.first[indices[i]] = j;

				clusters = solucion_to_clusters(sol_intermedia.first);
				calcular_desviacion_general();

				sol_intermedia.second = funcion_objetivo();
				evaluaciones++;

				if (sol_intermedia.second < val_mejor_cluster){
					mejor_cluster = j;
					val_mejor_cluster = sol_intermedia.second;
					mejora = true;
				}
			}
		}

		if (!mejora){
			fallos++;
		} else {
			contador[sol_ini.first[indices[i]]]--;
			sol_ini.first[indices[i]] = mejor_cluster;
			sol_ini.second = val_mejor_cluster;
			contador[mejor_cluster]++;
		}

		i++;
	}

	//std::cout << "ACABAMOS CON: " << sol_ini.second << std::endl;
	//::cout << std::endl << std::endl << std::endl;

	return evaluaciones;

}



double PAR::funcion_objetivo() const {
	return (get_desviacion_general() + (calcular_infactibilidad() * get_lambda()));
}














/*

	PRACTICA 3


*/

std::pair<std::vector<PAR::Cluster>, double> PAR::algoritmo_BMB(const int num_soluciones,
																			  		 const int num_ite_solucion){

	//algoirtmo búsqueda multiarranque básico

	std::vector<std::vector<PAR::Cluster>> soluciones;

	for (int i = 0; i < num_soluciones; i++){
		soluciones.push_back(generar_solucion_aleatoria());
	}

	std::pair<std::vector<PAR::Cluster>, double> mejor;
	mejor.second = std::numeric_limits<double>::infinity();

	for (int i = 0; i < num_soluciones; i++){
		int iteraciones = num_ite_solucion;
		auto sol_i = algoritmo_BL(soluciones[i], iteraciones);

		if (sol_i.second < mejor.second){
			mejor = sol_i;
		}
	}

	clusters = mejor.first;
	calcular_desviacion_general();
	return mejor;

}


std::pair<std::vector<PAR::Cluster>, double> PAR::algoritmo_ES(const std::vector<PAR::Cluster> & ini,
																					const unsigned TOPE_EVALUACIONES,
																					const double prob_sea_peor,
																					const double prob_aceptar_peor){

	clusters = ini;
	calcular_desviacion_general();
	std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> solucion_actual = std::make_pair(std::make_pair(ini, funcion_objetivo() ), calcular_infactibilidad() );
	std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> mejor_solucion = solucion_actual;
	std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> vecino;

	// numero máximo de vecinos para el bucle interno, 10 por el número de datos del problema
	const unsigned MAX_VECINOS = 10 * datos.size();
	const unsigned MAX_EXITOS  = 0.1 * MAX_VECINOS;

	const double NUM_ENFRIAMIENTOS_M = TOPE_EVALUACIONES / MAX_VECINOS;

	unsigned evaluaciones = 0;


	const double TEMPERATURA_INICIAL = (prob_sea_peor * solucion_actual.first.second) / -log(prob_aceptar_peor);

	const double TEMPERATURA_FINAL = 0.001;

	double temperatura = TEMPERATURA_INICIAL;

	// inicializo a 1 para que entre en el bucle, luego se ponen a 0
	unsigned num_vecinos = 1;
	unsigned exitos = 1;
	//std::cout << "Temperatura inicial: " << temperatura << std::endl;

	while (evaluaciones < TOPE_EVALUACIONES && exitos != 0 && temperatura > TEMPERATURA_FINAL){

		num_vecinos = 0;
		exitos = 0;

		while (num_vecinos < MAX_VECINOS && exitos < MAX_EXITOS){
			// generamos vecino
			vecino = generar_vecino_es(solucion_actual);
			evaluaciones++;
			num_vecinos++;

			// calculamos diferencia de funcion objetivo
			double diferencia = vecino.first.second - solucion_actual.first.second;

			// si la solucion es mejor o si con probabilidad uniforme (0,1) <= criterio metropolis
			if (diferencia < 0 || Rand() <= exp( -diferencia/temperatura )){
				// cambiamos a esa solucion
				solucion_actual = vecino;
				exitos++;
				// si el coste de esa solucion es mejor que la mejor solucion, actualizamos mejor_solucoon
				if (solucion_actual.first.second < mejor_solucion.first.second){
					mejor_solucion = solucion_actual;
				}
			}

		}

		// esquema de enfriamiento
		temperatura = esquema_enfriamiento(temperatura, TEMPERATURA_INICIAL, TEMPERATURA_FINAL, NUM_ENFRIAMIENTOS_M);
		//std::cout << temperatura << std::endl;
	}

	clusters = mejor_solucion.first.first;
	calcular_desviacion_general();
	return mejor_solucion.first;
}




double PAR::esquema_enfriamiento(const double temperatura, const double temperatura_inicial, const double temperatura_final,
											const double M) const {
	const double BETA = (temperatura_inicial - temperatura_final) / (M * temperatura_inicial * temperatura_final);

	return temperatura / (1 + BETA * temperatura);
	//return temperatura * 0.92;

}



std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> PAR::generar_vecino_es(const std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> & ini){

	std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> vecino = ini;

	bool vecino_valido = false;

	clusters = vecino.first.first;

	do {
		int elemento_cambiar = RandPositiveInt(datos.size());

		int num_cluster = buscar_elemento(elemento_cambiar);

		// si puedo sacar el elemento del cluster
		if (vecino.first.first[num_cluster].num_elementos() - 1 > 0){
			int nuevo_cluster;
			do {
				nuevo_cluster = RandPositiveInt(get_num_clusters());
			} while (nuevo_cluster == num_cluster);

			vecino.first.first[num_cluster].delete_elemento( elemento_cambiar );

			vecino.second -= cumple_restricciones( elemento_cambiar, num_cluster );
			vecino.second += cumple_restricciones( elemento_cambiar, nuevo_cluster );


			vecino.first.first[nuevo_cluster].add_elemento( elemento_cambiar );

			clusters = vecino.first.first;
			calcular_desviacion_general();
			vecino.first.second = get_desviacion_general() + (vecino.second * get_lambda());


			vecino_valido = true;
		}

	} while (!vecino_valido);

	return vecino;

}







std::pair<std::vector<PAR::Cluster>, double> PAR::algoritmo_ILS(const std::vector<PAR::Cluster> & ini,
																			  		 const unsigned IT_BL,
																				 	 const unsigned IT_ILS,
																				 	 const double cambio_mutacion){

	clusters = ini;
	calcular_desviacion_general();
	std::pair<std::vector<PAR::Cluster>, double> sol_ini = std::make_pair(ini, funcion_objetivo());

	std::pair<std::vector<PAR::Cluster>, double> mejor_sol = sol_ini;

	int iteraciones_BL = IT_BL;

	sol_ini = algoritmo_BL(sol_ini.first, iteraciones_BL);

	if (sol_ini.second > mejor_sol.second){
		mejor_sol = sol_ini;
	}


	unsigned evaluaciones = 0;

	while (evaluaciones < IT_ILS) {

		sol_ini = operador_mutacion_segmento_fijo(mejor_sol, cambio_mutacion);

		iteraciones_BL = IT_BL;
		sol_ini = algoritmo_BL(sol_ini.first, iteraciones_BL);
		evaluaciones += iteraciones_BL;

		if (sol_ini.second < mejor_sol.second){
			mejor_sol = sol_ini;
		}

	}


	clusters = mejor_sol.first;
	calcular_desviacion_general();
	return mejor_sol;

}



std::pair<std::vector<PAR::Cluster>, double> PAR::operador_mutacion_segmento_fijo(const std::pair<std::vector<PAR::Cluster>, double> & ini,
																									  const double porcentaje_cambiar){

	std::vector<int> original = clusters_to_solucion(ini.first);
	std::vector<int> mutacion = original;
	std::vector<Cluster> solucion;

	unsigned INI_SEGMENTO = RandPositiveInt(original.size());

	unsigned TAM_SEGMENTO = original.size() * porcentaje_cambiar;

	int nuevo_cluster = -1;

	for (unsigned i = INI_SEGMENTO; i < ((INI_SEGMENTO + TAM_SEGMENTO) % original.size()) - 1; i++){

		do {
			nuevo_cluster = RandPositiveInt(get_num_clusters());
		} while (nuevo_cluster == original[i]);

		mutacion[i] = nuevo_cluster;

	}

	solucion = solucion_to_clusters(mutacion);
	clusters = solucion;
	calcular_desviacion_general();
	return std::make_pair(solucion, funcion_objetivo());

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
