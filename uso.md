# Indicaciones de uso del programa

## Compilación

Para compilar usar:


```bash
make
```
Esto crea un archivo ejecutable de nombre `external_sort`.

## Uso

El programa provee distintos comandos:

### Búsqueda de aridad óptima

Comando para encontrar la aridad óptima para los algoritmos de ordenamiento:

```bash
make find-arity
```

o

```bash
./external_sort find_arity
```

Esto ejecutará una búsqueda binaria para encontrar el mejor valor de aridad para Mergesort Externo, el que será utilizado también con Quicksort Externo.

### Ejecutar experimentos con aridad específica

Para ejecutar experimentos con un valor específico de aridad:

```bash
./external_sort run_tests <a>
```

Por ejemplo:

```bash
./external_sort run_tests 8
```

### Ejecutar el experimento completo

Para encontrar la aridad óptima y luego realizar todos los experimentos:

```bash
make full
```

o

```bash
./external_sort full
```

## Detalles del experimento

Los experimentos:

1. Generan 5 secuencias aleatorias de enteros de 64 bits para cada valor de tamaño $N$, con $N \in \{4M, 8M, \dots , 60M\}$
2. Ordenan cada secuencia utilizando ambos algoritmos
3. Miden el tiempo de ejecución y el número de accesos a disco
4. Calculan el promedio de los resultados de las 5 ejecuciones para cada tamaño de $N$
5. Guardan los resultados en un archivo CSV

## Uso con Docker


Para ejecutar los experimentos en un ambiente controlado con memoria limitada:

1. Crear la imagen de Docker:
```bash
docker build -t external_sort .
```

2. Ejectuar el contenedor con memoria limitada:
```bash
docker run --memory=60m -it external_sort
```

Dentro del contenedor, es posible ejecutar los distintos comandos descritos antes.

## Limpieza

Para eliminar los archivos creados al compilar:

```bash
make clean
```

Para eliminar además los archivos creados (.bin):

```bash
make clean-all
```