#ifndef PAR_H_INCLUDED
#define PAR_H_INCLUDED

#include <vector>
#include <set>
#include <map>
#include <string>
#include <ostream>
#include <list>


enum class operador_cruce {SEGMENTO_FIJO, UNIFORME};
enum class tipo_generacion {GENERACIONAL, ESTACIONARIO};

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
			 const int num_clusters);


		int get_num_clusters() const;
		double get_desviacion_general() const;
		double get_mayor_distancia() const;
		double get_lambda() const;

		int buscar_cluster(const int elemento);

		int buscar_elemento(const int elemento) const;
		void calcular_desviacion_general();
		std::pair<std::vector<PAR::Cluster>,int> algoritmo_greedy();
		std::pair<std::vector<PAR::Cluster>,int> algoritmo_BL(const std::vector<Cluster> & ini);
		std::pair<std::vector<PAR::Cluster>,int> algoritmos_AG(const unsigned evaluaciones_max,\
			 																	 const unsigned tam_pob_ini,\
																				 const float prob_mutacion, \
																				 const float prob_cruce,
																			 	 const operador_cruce tipo_cruce,
																				 const tipo_generacion tipo_gen,
																			 	 const bool elitismo = true);

		int calcular_infactibilidad() const;
		std::vector<Cluster> generar_solucion_aleatoria();
		friend std::ostream & operator << (std::ostream & flujo, const PAR & par);



	private:
		std::vector<Cluster> clusters;
		std::map< std::pair<int, int>, int> restricciones;
		std::vector< std::vector<double> > datos;
		std::list< std::tuple<int, int, int> > lista_restricciones;

		double desviacion_general;

		double mayor_distancia;
		double LAMBDA;

		double distancia_puntos(const std::vector<double> & p1,
									 	const std::vector<double> & p2) const;

		std::vector<int> clusters_to_solucion(const std::vector<PAR::Cluster> & clusters_ini) const;
		std::vector<PAR::Cluster> solucion_to_clusters(const std::vector<int> & sol);
		std::vector<std::vector<int>> generar_poblacion_inicial(const int tam_pob_ini);
		int cumple_restricciones(const int elemento, const int num_cluster);
		void leer_datos(const std::string fichero);
		void leer_restricciones(const std::string fichero);

		std::vector<std::pair<std::vector<int>, double>> seleccion_AGG(const std::vector<std::pair<std::vector<int>, double>> & poblacion);
		std::vector<std::pair<std::vector<int>, double>> seleccion_AGE(const std::vector<std::pair<std::vector<int>, double>> & poblacion);

		unsigned operador_cruce_uniforme(std::vector<std::pair<std::vector<int>, double>> & poblacion,\
			  								  const double prob_cruce);

		unsigned operador_cruce_seg_fijo(std::vector<std::pair<std::vector<int>, double>> & poblacion,\
			  								  const double prob_cruce);

		void reparar_cruce(std::vector<int> & reparado);

		unsigned operador_mutacion_uniforme(std::vector<std::pair<std::vector<int>, double>> & poblacion ,
	 												   const double prob_mut,
												  	   const tipo_generacion tipo_g);

		int algoritmo_BL_suave(std::vector<int> & sol_ini,
			 						  const unsigned fallos_permitidos);


		static const int TOPE_BL = 100000;



};


#endif
