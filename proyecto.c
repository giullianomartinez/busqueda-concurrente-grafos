#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int grafo[30][30];
int nodosarecorrer[5];
int nodoinicial;

void lee_grafo()
{
    FILE *arch;
    char linea[160], *p;
    int i = 0, j, k;

    arch = fopen("grafo.csv", "r");
    while (fgets(linea, 160, arch) != NULL)
    {
        if (i == 0) /* lee primera linea = Nodo inicial */
        {
            nodoinicial = atoi(linea);
            printf("nodo inicio  = %d \n", nodoinicial);
            i++;
        }
        else
        {
            linea[strlen(linea) - 1] = ';'; /* reemplaza el \n por ; para que el strtok tome el ultimo numero */
            if (i == 1)                     /* Segunda linea = Nodos a recorrer */
            {
                p = strtok(linea, ";"); /* encuento el string anterior a un ; que corresponde al nodo */
                for (j = 0; j < 5; j++)
                {
                    nodosarecorrer[j] = atoi(p); /* lo almaceno en el vector nodosarecorrer */
                    printf("%d \n", nodosarecorrer[j]);
                    p = strtok(NULL, ";"); /* busco el sigiente ; en el string */
                }
                i++;
            }
            else /* Desde tercera linea , por cada nodo  lista  sus enlaces */
            {
                p = strtok(linea, ";"); /* saco el nodo del string */
                j = atoi(p) - 1;
                k = 0;
                while ((p = strtok(NULL, ";")) != NULL) /* busco cada nodo apuntado que está separado por ; */
                {
                    grafo[j][k++] = atoi(p); /* en la matriz grafo, dejo cada enlace del nodo */
                }
                grafo[j][k] = -1; /* inserto al final de la fila un -1 para indicar que no hay mas enlace */
            }
        }
    }
}

void imprime_grafo()
{
    int i, j;
    for (i = 0; i < 30; i++)
    {
        printf("\n nodo %d -> ", i + 1);
        for (j = 0; grafo[i][j] != -1; j++)
            printf(" - %d", grafo[i][j]);
    }
}

int main()
{

    lee_grafo();
    imprime_grafo();
}
