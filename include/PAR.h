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
				const std::vector<double> & get_centroide() const;

				const std::set<int> & get_elementos() const;
				void add_elemento(const int elemento);
				void delete_elemento(const int elemento);

		};


		PAR(const std::string fichero_datos, const std::string fichero_restricciones,
			 const int num_clusters);
		void leer_datos(const std::string fichero);
		void leer_restricciones(const std::string fichero);

		int get_num_clusters() const;
		void set_num_clusters(const int n_num_clusters);

		std::vector<Cluster> algoritmo_COPKM();
		int buscar_cluster(const int elemento);

	private:
		std::vector<Cluster> clusters;
		//std::vector< std::vector<int> > restricciones;
		std::map< std::pair<int, int>, int> restricciones;
		std::vector< std::vector<double> > datos;

		int num_clusters;

		double distancia_puntos(const std::vector<double> p1,
									 	const std::vector<double> p2);

		std::vector<int> clusters_to_solucion();
		bool cumple_restricciones(const int elemento, const int num_cluster);


};


#endif
