# Tarea 1 Diseno Analisis Algoritmos

## <u>Descripción</u>
Este repositorio contiene las implementaciones de algoritmos de ordenamiento externo (External MergeSort y QuickSort) y el calculo de aridad óptima desarrollados para el curso CC4102 - Diseño y Análisis de Algoritmos.

## <u>Requisitos</u>
- Compilador C++17 (g++ 7 o superior recomendado).
- Sistema operativo Linux (probado en Ubuntu 20.04).
- 2GB de espacio libre en disco (para archivos temporales).
- 4GB de RAM recomendados.

## <u>Para ejecutar el experimento</u>
Para realizar el experimento:
1) Sencillamente se debe clonar el repositorio con `git clone <url de la repo>`.
2) Una vez dentro del repositorio, realizar en la consola `make run experiment` el cual ejecuta `experiment.c`.
3) Si quieren utilizar el entorno de de docker seguir las indicaciones del siguiente link: https://hub.docker.com/r/pabloskewes/cc4102-cpp-env.

Sugerimos que no ocupen 50mb como indica la tarea. En el informe se detalla mas al respecto, nosotros utilizamos 65mb que es lo minimo que nos sirvio para correr el experimento.
Si desean, esta habilitado `make clean`.

Para realizar el calculo de la aridad:
1) En la terminal colocar:  `g++ -std=c++17 -Wall -O0 -I. arity.cpp mergesort.cpp iostats.cpp -lstdc++fs -o arity`.
2) Luego ejecutar: `./arity`.