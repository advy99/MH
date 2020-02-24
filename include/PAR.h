#ifndef PAR_H_INCLUDED
#define PAR_H_INCLUDED

//#include "Cluster.h"
#include <vector>
#include <set>
#include <string>

class PAR{

	public:
		PAR(const std::string fichero_datos, const std::string fichero_restricciones,
			 const int num_clusters);
		void leerDatos(const std::string fichero);
		void leerRestricciones(const std::string fichero);

		int getNumClusters() const;


		class Cluster{
			private:
				std::vector<double> centroide;
				std::set<int> elementos;

			public:
				Cluster();
				void calcularCentroide();

				void setCentroide(const std::vector<double> n_centroide);
				std::pair<double, double> getCentroide() const;

				std::set<int> getElementos() const;
				void addElemento(const int elemento);
				void deleteElemento(const int elemento);

		};


	private:
		std::vector< Cluster > clusters;
		std::vector< std::vector<int> > restricciones;
		std::vector< std::vector<double> > datos;

		int num_clusters;

		void setNumClusters(const int n_num_clusters);

};


#endif
