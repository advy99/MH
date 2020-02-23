#ifndef CLUSTER_H_INCLUDED
#define CLUSTER_H_INCLUDED


class Cluster{
	private:
		std::vector<double> centroide;
		std::set<int> elementos;

	public:
		Cluster();
		void calcularCentroide();

		void setCentroide(const pair<double, double> n_centroide);
		pair<double, double> getCentroide() const;

		std::set<int> getElementos() const;
		void addElemento(const int elemento);
		void deleteElemento(const int elemento);

};



#endif
