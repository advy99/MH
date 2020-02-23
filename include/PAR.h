#ifndef PAR_H_INCLUDED
#define PAR_H_INCLUDED

//#include "Cluster.h"
#include <vector>
#include <set>
#include <string>

class PAR{
	private:
		//std::vector<Cluster> clusters;
		std::vector< std::vector<int> > restricciones;
		std::vector< std::vector<double> > datos;

		const int num_clusters;


	public:
		PAR(const std::string fichero_datos, const std::string fichero_restricciones,
			 const int num_clusters);
		void leerDatos(const std::string fichero);
		void leerRestricciones(const std::string fichero);


};


#endif
