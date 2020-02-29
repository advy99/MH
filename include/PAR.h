#ifndef PAR_H_INCLUDED
#define PAR_H_INCLUDED

#include <vector>
#include <set>
#include <map>
#include <string>

class PAR{

	public:

		class Cluster{
			private:

				std::vector<double> centroide;
				std::vector<double> distancia_intra_cluster;

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

				Cluster & operator = (const Cluster & otro);


		};


		PAR(const std::string fichero_datos, const std::string fichero_restricciones,
			 const int num_clusters);


		int get_num_clusters() const;
		void set_num_clusters(const int n_num_clusters);

		int buscar_cluster(const int elemento);
		void calcular_desviacion_general();
		std::vector<Cluster> algoritmo_COPKM();
		std::vector<Cluster> algoritmo_BL();


	private:
		std::vector<Cluster> clusters;
		//std::vector< std::vector<int> > restricciones;
		std::map< std::pair<int, int>, int> restricciones;
		std::vector< std::vector<double> > datos;
		double desviacion_general;

		int num_clusters;

		double distancia_puntos(const std::vector<double> & p1,
									 	const std::vector<double> & p2);

		std::vector<int> clusters_to_solucion();
		bool cumple_restricciones(const int elemento, const int num_cluster);
		void leer_datos(const std::string fichero);
		void leer_restricciones(const std::string fichero);


};


#endif
