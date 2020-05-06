#ifndef PAR_H_INCLUDED
#define PAR_H_INCLUDED

#include <vector>
#include <set>
#include <map>
#include <string>
#include <ostream>
#include <list>


enum class operador_cruce {SEGMENTO_FIJO, UNIFORME};
enum class tipo_generacion {GENERACIONAL, ESTACIONARIO, MEMETICO_1, MEMETICO_0_1, MEMETICO_0_1_MEJ, MEMETICO_BL_1, MEMETICO_BL_0_1, MEMETICO_BL_0_1_MEJ};

class PAR{

	public:

		class Cluster{
			private:

				std::vector<double> centroide;
				double distancia_intra_cluster;

				std::set<int> elementos;

				PAR& problema;


			public:
				Cluster( PAR & p);
				void calcular_centroide();
				void calcular_distancia_intra_cluster();


				void set_centroide(const std::vector<double> n_centroide);
				double get_distancia_intra_cluster() const;
				const std::vector<double> & get_centroide() const;

				const std::set<int> & get_elementos() const;
				void add_elemento(const int elemento);
				void delete_elemento(const int elemento);
				void limpiar();
				unsigned num_elementos() const;

				Cluster & operator = (const Cluster & otro);
				friend std::ostream & operator << (std::ostream & flujo, const Cluster & c);

		};


		PAR(const std::string fichero_datos, const std::string fichero_restricciones,
			 const int num_clusters, const std::string semilla);


		int get_num_clusters() const;
		double get_desviacion_general() const;
		double get_mayor_distancia() const;
		double get_lambda() const;

		int buscar_cluster(const int elemento);

		int buscar_elemento(const int elemento) const;
		void calcular_desviacion_general();
		std::pair<std::vector<PAR::Cluster>,double> algoritmo_greedy();
		std::pair<std::vector<PAR::Cluster>,double> algoritmo_BL(const std::vector<Cluster> & ini, int & TOPE_BL, const bool salida = false);
		std::pair<std::vector<PAR::Cluster>,double> algoritmos_AG(const unsigned evaluaciones_max,\
			 																	 const unsigned tam_pob_ini,\
																				 const float prob_mutacion, \
																				 const float prob_cruce,
																			 	 const operador_cruce tipo_cruce,
																				 const tipo_generacion tipo_gen,
																			 	 const bool elitismo = true);

		std::pair<std::vector<PAR::Cluster>, double> algoritmo_BMB(const int num_soluciones,
																					  const int num_ite_solucion);

		std::pair<std::vector<PAR::Cluster>, double> algoritmo_ES(const std::vector<PAR::Cluster> & ini,
																					 const unsigned TOPE_EVALUACIONES,
																					 const double prob_sea_peor,
																					 const double prob_aceptar_peor);


		std::pair<std::vector<PAR::Cluster>, double> algoritmo_ILS(const std::vector<PAR::Cluster> & ini,
																			  		 const unsigned IT_BL,
																				 	 const unsigned IT_ILS,
																				 	 const double cambio_mutacion);


		int calcular_infactibilidad() const;
		std::vector<Cluster> generar_solucion_aleatoria();
		friend std::ostream & operator << (std::ostream & flujo, const PAR & par);

		std::string NOM_DATOS;
		std::string NOM_RESTRICCIONES;
		std::string SEMILLA;


	private:
		std::vector<Cluster> clusters;
		std::map< std::pair<int, int>, int> restricciones;
		std::vector< std::vector<double> > datos;
		std::list< std::tuple<int, int, int> > lista_restricciones;
		std::list< std::tuple<int, int, int> > lista_restricciones_superior;


		double desviacion_general;

		double mayor_distancia;
		double LAMBDA;

		double funcion_objetivo() const;
		double distancia_puntos(const std::vector<double> & p1,
									 	const std::vector<double> & p2) const;

		std::vector<int> clusters_to_solucion(const std::vector<PAR::Cluster> & clusters_ini) const;
		std::vector<PAR::Cluster> solucion_to_clusters(const std::vector<int> & sol);
		std::vector<std::vector<int>> generar_poblacion_inicial(const unsigned tam_pob_ini);
		int cumple_restricciones(const int elemento, const int num_cluster);
		void leer_datos(const std::string fichero);
		void leer_restricciones(const std::string fichero);

		std::vector<std::pair<std::vector<int>, double>> seleccion_algoritmos_ev(const std::vector<std::pair<std::vector<int>, double>> & poblacion, const int tam);

		unsigned operador_cruce_uniforme(std::vector<std::pair<std::vector<int>, double>> & poblacion,\
			  								  		const double prob_cruce);

		unsigned operador_cruce_seg_fijo(std::vector<std::pair<std::vector<int>, double>> & poblacion,\
			  								  		const double prob_cruce);

		void reparar_cruce(std::vector<int> & reparado);

		unsigned operador_mutacion_uniforme(std::vector<std::pair<std::vector<int>, double>> & poblacion ,
	 												   const double prob_mut,
												  	   const tipo_generacion tipo_g);

		int algoritmo_BL_suave(std::pair<std::vector<int>, double> & sol_ini,
			 						  const unsigned fallos_permitidos);


		//static const int TOPE_BL = 100000;
		double esquema_enfriamiento(const double temperatura, const double temperatura_inicial,
			 								 const double temperatura_final, const double M) const;

		std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> generar_vecino_es(const std::pair<std::pair<std::vector<PAR::Cluster>, double>, int> & ini);

		std::pair<std::vector<PAR::Cluster>, double> operador_mutacion_segmento_fijo(const std::pair<std::vector<PAR::Cluster>, double> & ini,
																											  const double porcentaje_cambiar);

};


#endif
