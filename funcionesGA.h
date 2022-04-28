#ifndef funcionesGA
#define funcionesGA

	void minIvn(double *vector, int tam, /*Outputs: */ double *min, int *index);

	void ordenarMin2Max(int *poblacion, int tamPoblacion, int numPozos, double *costoPoblacion);

	void genPoblacion(int numPozos, int numClusters, int tamPoblacion, /*Out*/ int *poblacion);
	
	void funcionCosto(double *posPozos, int numPozos, double *posClusters, int numClusters, int *poblacion, int tamPoblacion, double *liqPozo,
				  double *maxLiqClusters, double *minLiqCluster, double penalizacion, double *distanciaCompletamientos, double costoPieTuberia,
   			      double costoCluster, double *clusterNuevo , /*Outputs: */ double *costoPoblacion);

	void funcionCostoMejor(double *posPozos, int numPozos, double *posClusters, int *mejorIndividuo, double *distanciaCompletamientos, /*Outputs*/ double *distanciasPozos);

	void mutacion(int *poblacion, int tamPoblacion, int numPozos, int numClusters);

	void cruce(int poblacion, int tamPoblacion, int numPozos);

	void optimizarGA(double *posPozos, int numPozos, double *liqPozo, double *posClusters, int numClusters, double *maxLiqCluster,  double *minLiqCluster, double dx, double dy,
				 double dz, int numGeneraciones, int tamPoblacion, double penalizacion, int totalGA, double *distanciaCompletamientos, double costoPieTuberia, double costoCluster,
				 double *clusterNuevo, /*Outputs:*/ double *mejorCosto, int *mejorIndividuo, double *distanciasPozos);

#endif