# Búsqueda Concurrente de Rutas en C

Proyecto desarrollado para el ramo **Hardware y Sistemas Operativos**.

El programa busca la ruta más corta dentro de un grafo, pasando por una serie de nodos obligatorios. La exploración se realiza de forma concurrente utilizando **POSIX Threads (`pthread`)**, mutex y semáforos.

## Características

* Lectura del grafo desde `grafo.csv`.
* Búsqueda concurrente de rutas.
* Máximo de 40 hebras simultáneas.
* Evita repetir nodos dentro de una ruta.
* Protege recursos compartidos mediante mutex.

## Compilación

```bash
gcc proyecto-Kali.c -o proyecto -pthread
```

## Ejecución

```bash
./proyecto
```

El programa lee automáticamente `grafo.csv` y muestra la mejor ruta encontrada, su cantidad de nodos y el total de hebras creadas.

## Tecnologías

* C
* pthread
* Semáforos POSIX
* Mutexes
* Grafos
* Programación concurrente
