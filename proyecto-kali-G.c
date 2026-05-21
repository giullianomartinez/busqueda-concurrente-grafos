/*
 * INF2322 - Paralelo 1
 * Taller 1
 *
 * INTEGRANTES: Giulliano Martinez y Benjamin Huenuvil
 */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NODOS 30
#define MAX_OBLIGATORIOS 5
#define MAX_HEBRAS 40

int grafo[MAX_NODOS][MAX_NODOS];
int nodosObligatorios[MAX_OBLIGATORIOS];
int nodoinicial;

struct Ruta {
    int nodos[MAX_NODOS];
    int visitados[MAX_NODOS];
    int largo;
};

struct ArgumentoHebra {
    int actual;
    int numeroHebra;
    struct Ruta ruta;
};

struct Ruta mejorRutaGlobal;
int hayMejorRuta = 0;

int contadorHebras = 0;
int hebrasActivas = 0;

pthread_mutex_t mutexMejorRuta = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexImpresion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexContadorHebras = PTHREAD_MUTEX_INITIALIZER;

sem_t semHebras;


void inicializarGrafo(){
    int fila, columna;
    for(fila = 0; fila < MAX_NODOS; fila++){
        for(columna = 0; columna < MAX_NODOS; columna++){
            grafo[fila][columna] = -1;
        }
    }
}


void inicializarRuta(struct Ruta *ruta){
    int i;
    ruta->largo = 0;
    for(i = 0; i < MAX_NODOS; i++){
        ruta->nodos[i] = 0;
        ruta->visitados[i] = 0;
    }
}


int todosObligatoriosVisitados(int visitados[]){
    int i;
    int nodoObligatorio;

    for(i = 0; i < MAX_OBLIGATORIOS; i++){
        nodoObligatorio = nodosObligatorios[i];

        if(nodoObligatorio < 1 || nodoObligatorio > MAX_NODOS){
            return 0;
        }
        if(visitados[nodoObligatorio - 1] == 0){
            return 0;
        }
    }
    return 1;
}


void lee_grafo()
{
  FILE * arch;
  char linea  [160], * p, *q;
  int i=0,j,k;


  arch = fopen("grafo.csv","r");
  while ( fgets(linea,160,arch) != NULL)
  {
      if ( i==0) /* lee primera linea = Nodo inicial */
      {
	 nodoinicial = atoi(linea);
	 printf("nodo inicio  = %d \n", nodoinicial);
         i++;
       }else
	{ 
          linea[strlen(linea)-1]=';';     /* reemplaza el \n por ; para que el strtok tome el ultimo numero */		
	  if (i == 1)  /* Segunda linea = Nodos a recorrer */
          {
	     p = strtok(linea,";");   /* encuento el string anterior a un ; que corresponde al nodo */
             for(j=0;j<5;j++)	
	     {	
		nodosarecorrer[j] = atoi(p);     /* lo almaceno en el vector nodosarecorrer */
	        printf("%d \n",nodosarecorrer[j]);
		p = strtok(NULL,";");        /* busco el sigiente ; en el string */
	     }  
	     i++; 
	  } 
           else   /* Desde tercera linea , por cada nodo  lista  sus enlaces */
           {  
	     p = strtok(linea,";");  /* saco el nodo del string */
             j = atoi(p) - 1;
	     k = 0;
             while ((p =strtok(NULL,";")) != NULL)  /* busco cada nodo apuntado que está separado por ; */
	     {
		 grafo [j][k++]= atoi(p);    /* en la matriz grafo, dejo cada enlace del nodo */
	     }
	     grafo[j][k] = -1;  /* inserto al final de la fila un -1 para indicar que no hay mas enlace */
	   }
	}
  }
 }


void imprime_grafo(){
    int fila, columna;

    printf("\nGRAFO CARGADO:\n");

    for(fila = 0; fila < MAX_NODOS; fila++){
        printf("\nNodo %d -> ", fila + 1);

        for(columna = 0; columna < MAX_NODOS && grafo[fila][columna] != -1; columna++){
            printf("%d ", grafo[fila][columna]);
        }
    }
    printf("\n\n");
}


int rutaEsMejor(struct Ruta *actual, struct Ruta *mejorRuta){
    return actual->largo < mejorRuta->largo;
}


void imprimirRuta(struct Ruta *ruta){
    int i;

    for(i = 0; i < ruta->largo; i++){
        printf("%d", ruta->nodos[i]);

        if(i < ruta->largo - 1){
            printf(" -> ");
        }
    }

    printf("\nCantidad de nodos: %d", ruta->largo);
}


int obtenerNumeroHebra(){
    int numero;

    pthread_mutex_lock(&mutexContadorHebras);

    contadorHebras++;
    hebrasActivas++;

    numero = contadorHebras;

    pthread_mutex_unlock(&mutexContadorHebras);

    return numero;
}


int obtenerHebrasActivas(){
    int activas;

    pthread_mutex_lock(&mutexContadorHebras);

    activas = hebrasActivas;

    pthread_mutex_unlock(&mutexContadorHebras);

    return activas;
}


void terminarHebraActiva(){
    pthread_mutex_lock(&mutexContadorHebras);

    hebrasActivas--;

    pthread_mutex_unlock(&mutexContadorHebras);
}


void imprimirRutaEncontrada(struct Ruta *ruta, int numeroHebra){
    int i;
    int hebrasActivasActuales;

    hebrasActivasActuales = obtenerHebrasActivas();

    pthread_mutex_lock(&mutexImpresion);

    printf("\n[Hebras activas: %d] Ruta: ", hebrasActivasActuales);

    for(i = 0; i < ruta->largo; i++){
        printf("%d", ruta->nodos[i]);

        if(i < ruta->largo - 1){
            printf(" -> ");
        }
    }

    printf(" | Nodos: %d", ruta->largo);

    pthread_mutex_unlock(&mutexImpresion);
}


void guardarMejorRuta(struct Ruta *actual, int numeroHebra){
    int i;
    int hebrasActivasActuales;

    pthread_mutex_lock(&mutexMejorRuta);

    if(hayMejorRuta == 0 || rutaEsMejor(actual, &mejorRutaGlobal)){
        mejorRutaGlobal->largo = actual->largo;

        for(i = 0; i < actual->largo; i++){
            mejorRutaGlobal->nodos[i] = actual->nodos[i];
        }

        for(i = 0; i < MAX_NODOS; i++){
            mejorRutaGlobal->visitados[i] = actual->visitados[i];
        }

        hayMejorRuta = 1;

        hebrasActivasActuales = obtenerHebrasActivas();

        pthread_mutex_lock(&mutexImpresion);

        printf("\n[ID hebra %d | activas %d] NUEVA MEJOR RUTA:\n",numeroHebra,hebrasActivasActuales);

        for(i = 0; i < mejorRutaGlobal->largo; i++){
            printf("%d", mejorRutaGlobal->nodos[i]);

            if(i < mejorRutaGlobal->largo - 1){
                printf(" -> ");
            }
        }

        printf(" | Nodos: %d\n", mejorRutaGlobal->largo);

        pthread_mutex_unlock(&mutexImpresion);
    }

    pthread_mutex_unlock(&mutexMejorRuta);
}


void *funcionHebra(void *arg){
    struct ArgumentoHebra *datosHebra;
    int nodoActualHebra;
    int numeroHebra;
    struct Ruta *rutaHebra;

    datosHebra = (struct ArgumentoHebra *)arg;

    nodoActualHebra = datosHebra->actual;
    numeroHebra = datosHebra->numeroHebra;
    rutaHebra = datosHebra->ruta;

    free(datosHebra);

    buscarMejorRuta(nodoActualHebra, rutaHebra, numeroHebra);

    terminarHebraActiva();

    sem_post(&semHebras);

    return NULL;
}

void buscarMejorRuta(int nodoActual, struct Ruta *rutaActual, int numeroHebra){
    int i, nodoSiguiente, idNuevaHebra, cantidadHebras = 0;
    pthread_t hebras[MAX_NODOS];
    struct ArgumentoHebra *datosHebra;

    if(nodoActual < 1 || nodoActual > MAX_NODOS) return;
    if(rutaActual->largo >= MAX_NODOS) return;

    rutaActual->nodos[rutaActual.largo] = nodoActual;
    rutaActual->largo++;
    rutaActual->visitados[nodoActual - 1] = 1;

    if(todosObligatoriosVisitados(rutaActual->visitados)){
        imprimirRutaEncontrada(rutaActual, numeroHebra);
        guardarMejorRuta(rutaActual, numeroHebra);
        return;
    }
    
    pthread_mutex_lock(&mutexMejorRuta);
    if(hayMejorRuta == 1 && rutaActual->largo >= mejorRutaGlobal->largo){
        pthread_mutex_unlock(&mutexMejorRuta);
        return;
    }
    pthread_mutex_unlock(&mutexMejorRuta);

    for(i = 0; i < MAX_NODOS && grafo[nodoActual - 1][i] != -1; i++){
        nodoSiguiente = grafo[nodoActual - 1][i];

        if(nodoSiguiente < 1 || nodoSiguiente > MAX_NODOS) continue;
        if(rutaActual->visitados[nodoSiguiente - 1] == 1) continue;

        if(sem_trywait(&semHebras) == 0){
            datosHebra = malloc(sizeof(struct ArgumentoHebra));
            if(datosHebra == NULL){
                sem_post(&semHebras);
                buscarMejorRuta(nodoSiguiente, rutaActual, numeroHebra);
                continue;
            }

            idNuevaHebra = obtenerNumeroHebra();
            datosHebra->actual = nodoSiguiente;
            datosHebra->ruta = rutaActual;
            datosHebra->numeroHebra = idNuevaHebra;

            if(pthread_create(&hebras[cantidadHebras], NULL, funcionHebra, datosHebra) == 0){
                cantidadHebras++;
                continue;
            }
            free(datosHebra);
            terminarHebraActiva();
            sem_post(&semHebras);
            buscarMejorRuta(nodoSiguiente, rutaActual, numeroHebra);
        }else{
            buscarMejorRuta(nodoSiguiente, rutaActual, numeroHebra);
        }
    }

    for(i = 0; i < cantidadHebras; i++){
        pthread_join(hebras[i], NULL);
    }
}

void resolverMejorRuta(){
    struct Ruta *rutaActual;

    inicializarRuta(rutaActual);
    inicializarRuta(mejorRutaGlobal);

    hayMejorRuta = 0;
    contadorHebras = 0;
    hebrasActivas = 0;

    /*
     * ID hebra 0 representa la búsqueda inicial hecha por el hilo principal.
     * No cuenta dentro del semáforo porque no fue creada con pthread_create.
     */
    buscarMejorRuta(nodoinicial, rutaActual, 0);

    if(hayMejorRuta == 1){
        printf("\n\nMEJOR RUTA FINAL:\n");
        imprimirRuta(&mejorRutaGlobal);
        
    }else{
        printf("\nNo se encontro una ruta valida.\n");
    }

    printf("\nTotal de hebras creadas durante la ejecucion: %d\n", contadorHebras);
    printf("Hebras activas al finalizar: %d\n", obtenerHebrasActivas());
}


int main(){
    inicializarGrafo();
    lee_grafo();
    sem_init(&semHebras, 0, MAX_HEBRAS);
    resolverMejorRuta();
    sem_destroy(&semHebras);
    pthread_mutex_destroy(&mutexMejorRuta);
    pthread_mutex_destroy(&mutexImpresion);
    pthread_mutex_destroy(&mutexContadorHebras);
    return 0;
}
