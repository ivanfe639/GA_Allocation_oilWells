
#include <stdio.h> 	//printf, NULL
#include <stdlib.h> //malloc	
#include <math.h> //operadores matematicos
#include <string.h> //memcpy
#include <sys/time.h>

#include "funcionesGA.h"	

#define MAT(vec,i,j,num_col) (vec[(j) + num_col*(i)])    //macro para manejo de matrices arroja un solo valor

void minIvn(double *vector, int tam, /*Outputs: */ double *min, int *index){
    int i;
    *min = vector[0];
    *index = 0;

    for (i=1;i<tam;i++){
        if (vector[i]<=*min){
            *min = vector[i];
            *index = i;
        }
    }
}

void ordenarMin2Max(int *poblacion, int tamPoblacion, int numPozos, double *costoPoblacion){

	int i,index;
	double min;
	int  *swapTempPoblacion = (int *) malloc(numPozos*sizeof(int));
	double swapTemp;

	for (i=0;i<tamPoblacion;i++){
		minIvn(costoPoblacion + i,tamPoblacion-i, /*Outputs: */ &min, &index);
		swapTemp = costoPoblacion[0+i];
		memcpy(swapTempPoblacion,poblacion + i*numPozos,numPozos*sizeof(int));
		
		costoPoblacion[0+i] = costoPoblacion[index+i];
		memcpy(poblacion + i*numPozos,poblacion + (index+i)*numPozos,numPozos*sizeof(int));

		costoPoblacion[index+i] = swapTemp;
		memcpy(poblacion + (i+index)*numPozos,swapTempPoblacion,numPozos*sizeof(int));
				
	}
	free(swapTempPoblacion);
}


void genPoblacion(int numPozos, int numClusters, int tamPoblacion, /*Out*/ int  *poblacion){
	int i,j;
	struct timeval time;
	gettimeofday(&time,NULL);
	//srand((time.tv_sec * 1000) + (time.tv_usec / 100 ));
	for (i=0.;i<tamPoblacion;i++){				// Indice de individuos
		for (j=0;j<numPozos;j++){			// Indice de pozos  en el individuo
			MAT(poblacion,i,j,numPozos) = rand() % (numClusters) + 0;
		}
	}
}

//Funcion de costo para evaluar el fitness de cada individuo
void funcionCosto(double *posPozos, int numPozos, double *posClusters, int numClusters, int *poblacion, int tamPoblacion, double *liqPozo,
				  double *maxLiqClusters, double *minLiqCluster, double penalizacion, double *distanciaCompletamientos, double costoPieTuberia,
   			      double costoCluster, double *clusterNuevo , /*Outputs: */ double *costoPoblacion){

	int i,j;
	double tempCosto;
	double *tempCapacidad = (double *) malloc(numClusters*sizeof(double));

	for (i=0;i<tamPoblacion;i++){
		tempCosto = 0;
		//inicializa a cero la variable tempCapacidad se puede hacer con un memset
		for (j=0;j<numClusters;j++){
			tempCapacidad[j] = 0.0;
		}		

		for (j=0;j<numPozos;j++){
			//Se obtiene el costo por distancia 

			//El primer factor es el quie debe reemplazarse por la profundidad de la primera capa del pozo
			//FIX ME
			tempCosto = tempCosto + sqrt( pow(fabs( MAT(posPozos,j,2,3) ),2)  +   pow(fabs(MAT(posClusters,(int)MAT(poblacion,i,j,numPozos),0,2) - MAT(posPozos,j,0,3)),2)  +  pow(fabs(MAT(posClusters,(int)MAT(poblacion,i,j,numPozos),1,2) - MAT(posPozos,j,1,3)),2) );
			
			//Se le suma la distancia de los completamientos (distancia de tuberia que entra al yacimiento)
			tempCosto = tempCosto + distanciaCompletamientos[j];

			//Se obtiene la capacidad que debera soportar el cluster. Suma en sus pociciones los flujos del pozo para conocer la capacidad total que se obtuvo 
			tempCapacidad[(int)MAT(poblacion,i,j,numPozos)] = tempCapacidad[(int)MAT(poblacion,i,j,numPozos)] + liqPozo[j];
		}

		tempCosto = tempCosto*costoPieTuberia;		//[USD]

		// Restriccion flujo maximo
		for (j=0;j<numClusters;j++){
			if (tempCapacidad[j] > maxLiqClusters[j]){
				tempCosto = tempCosto + penalizacion;
			}
		}

		// Restriccion flujo minimo 
		for (j=0;j<numClusters;j++){
			if (tempCapacidad[j] < minLiqCluster[j]){
				tempCosto = tempCosto + penalizacion;
			}
		}

		// Comprueba si el cluster es nuevo para sumarle el costo de puesta en funcionamiento 
		for (j=0;j<numPozos;j++){
			if ( clusterNuevo[ (int)MAT(poblacion,i,j,numPozos) ] == 1 ){
				tempCosto = tempCosto + costoCluster;		// Suma el valor de la implementación de un cluster
			}
		}

		
		
		costoPoblacion[i] = tempCosto;
	}

	free(tempCapacidad);
}

// Obtiene el mejor valor 
void funcionCostoMejor(double *posPozos, int numPozos, double *posClusters, int *mejorIndividuo, double *distanciaCompletamientos, /*Outputs*/ double *distanciasPozos){

	int j;
	
	for (j=0;j<numPozos;j++){
		//Se obtiene el costo por distancia de cada pozo
		distanciasPozos[j] = sqrt( pow(fabs( MAT(posPozos,j,2,3) ),2)  +   pow(fabs(MAT(posClusters, (int)mejorIndividuo[j],0,2) - MAT(posPozos,j,0,3)),2)  +  pow(fabs(MAT(posClusters, (int)mejorIndividuo[j],1,2) - MAT(posPozos,j,1,3)),2) );
		distanciasPozos[j] = distanciasPozos[j] + distanciaCompletamientos[j];
	}

}	


// Condiciones de cambio
// Escoge un individuo y un gen para mutarlo
void mutacion(int *poblacion, int tamPoblacion, int numPozos, int numClusters){
	struct timeval time;
	gettimeofday(&time,NULL);
	//srand((time.tv_sec * 1000) + (time.tv_usec / 100 ));
	int individuo;
	int genMutar;
	int mutado;

	individuo = rand() % (tamPoblacion-1) + 1;
	genMutar = rand() % (numPozos) + 0;
	mutado = rand() % (numClusters) + 0;

	//printf("individuo = %i  genMutar = %i mutado a = %i \n",individuo,genMutar,mutado);

	MAT(poblacion,individuo,genMutar,numPozos) = mutado;
	
}

void cruce(int *poblacion, int tamPoblacion, int numPozos){
	struct timeval time;
	gettimeofday(&time,NULL);
	//srand((time.tv_sec * 1000) + (time.tv_usec / 100 ));
	int individuo1,individuo2;
	int genIntercambio;
	int tempGen;

	individuo1 = rand() % (tamPoblacion-1) + 1;		// Genera un numero aleatorio entre 1 - tamPoblacion para que no tome el primero (el mejor)
	individuo2 = rand() % (tamPoblacion-1) + 1;		// Genera un numero aleatorio entre 1 - tamPoblacion para que no tome el primero (el mejor)
	genIntercambio = rand() % (numPozos) + 0;		// Escoge el gen que va a intercambiar


	//printf("individuo1 = %i  individuo2 = %i genIntercambio = %i  \n",individuo1,individuo2,genIntercambio);

	tempGen = MAT(poblacion,individuo1,genIntercambio,numPozos);

	MAT(poblacion,individuo1,genIntercambio,numPozos) = MAT(poblacion,individuo2,genIntercambio,numPozos);
	MAT(poblacion,individuo2,genIntercambio,numPozos) = tempGen;
}


// En el algoritmo genetico se calculan las distancias de pozo, es decir la distacia de subsuelo + distancia dentro del yacimientos (completamientos)
void optimizarGA(double *posPozos, int numPozos, double *liqPozo, double *posClusters, int numClusters, double *maxLiqCluster,  double *minLiqCluster, double dx, double dy,
				 double dz, int numGeneraciones, int tamPoblacion, double penalizacion, int totalGA, double *distanciaCompletamientos, double costoPieTuberia, double costoCluster,
				  double *clusterNuevo, /*Outputs:*/ double *mejorCosto, int *mejorIndividuo, double *distanciasPozos){

	int i,j,w;

	double *posPozos2 = (double *) malloc(numPozos*3*sizeof(double));					// 2 columnas porque estos son la sposiciones i,j del pozo
	double *posClusters2 = (double *) malloc(numClusters*2*sizeof(double));				// 3 columnas porque estas son las posiciones i,j,k. i,y= posiciones X,Y. k=posiciones de profundidad
	int *poblacion = (int *) malloc(tamPoblacion*numPozos*sizeof(int));					// Poblacion de individuos 
	double *costoPoblacion = (double *) malloc(tamPoblacion*sizeof(double));
		
	//========================= Discretizacion del dx, dy y dz ===========================
	for (i=0;i<numPozos;i++){
		posPozos2[3*i + 0] = posPozos[3*i + 0]*dx;
		posPozos2[3*i + 1] = posPozos[3*i + 1]*dy;
		posPozos2[3*i + 2] = posPozos[3*i + 2]*dz;
	}
	for (i=0;i<numClusters;i++){
		posClusters2[2*i + 0] = posClusters[2*i + 0]*dx;
		posClusters2[2*i + 1] = posClusters[2*i + 1]*dy;
	}

	
	for (w=0;w<totalGA;w++){

		//========================= Generacion de la poblacion================================
		genPoblacion(numPozos,numClusters,tamPoblacion,/*output*/poblacion);

		//========================= Loop Generaciones ========================================
		for (i=0;i<numGeneraciones;i++){
			
			//Evaluacion de la funcion de costo
			funcionCosto(posPozos2,numPozos,posClusters2,numClusters,poblacion,tamPoblacion,liqPozo,maxLiqCluster,minLiqCluster,penalizacion,distanciaCompletamientos,costoPieTuberia,costoCluster,clusterNuevo, /*Outputs: */costoPoblacion);

			//Organizar de menor a mayor el vector de "costoPoblacion" y la matriz de individuos "poblacion"
			ordenarMin2Max(poblacion,tamPoblacion,numPozos,costoPoblacion);

			for (j=0;j<tamPoblacion/2;j++){
				mutacion(poblacion,tamPoblacion,numPozos,numClusters);
				cruce(poblacion,tamPoblacion,numPozos);

			}
			
		}

		if (w == 0){  //la primera iteracion
			*mejorCosto = costoPoblacion[0];
			memcpy(mejorIndividuo,poblacion + 0,numPozos*sizeof(int));

		}else if (*mejorCosto > costoPoblacion[0]){
			*mejorCosto = costoPoblacion[0];
			memcpy(mejorIndividuo,poblacion + 0,numPozos*sizeof(int));
		}
		
		printf("mejor Costo = %f \n",costoPoblacion[0]);
		printf("Mejor Individuo = [");
		for (i=0;i<numPozos;i++){
			printf(" %i ",poblacion[i]);
		}
		printf("]\n");
		
	}

	

	funcionCostoMejor(posPozos2,numPozos,posClusters2, mejorIndividuo, distanciaCompletamientos, /*Outputs*/distanciasPozos);

		
	free(posPozos2);
	free(posClusters2);
	free(poblacion);
	free(costoPoblacion);
}