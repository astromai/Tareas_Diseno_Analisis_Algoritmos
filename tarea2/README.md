<<<<<<< HEAD
# Tarea 2 - Algoritmo de Kruskal

## Descripción
Esta tarea presenta una implementación del algoritmo de Kruskal para obtener el árbol cobertor mínimo de un grafo. Se implementa también la interfaz Union-Find, para verificar la no formación de ciclos, etapa clave del algoritmo de Kruskal.
=======
# Tarea 2: Algoritmo de Kruskal

## Descripción

Este proyecto implementa el algoritmo de Kruskal para obtener el árbol cobertor mínimo (Minimum Spanning Tree, MST) de un grafo definido por puntos en el plano. Además se incluye la estructura **Union-Find** (Conjuntos Disjuntos) con **path compression** y **union by size** para evitar ciclos durante la construcción del MST.
>>>>>>> 64c8a31 (Update readme and add documentation)

Se han implementado cuatro variantes de Kruskal para comparar eficiencia:
1. Con ordenamiento de aristas y optimización de Union-Find.
2. Con ordenamiento de aristas sin optimización de Union-Find.
3. Con heap (priority_queue) y optimización de Union-Find.
4. Con heap sin optimización de Union-Find.

## Requisitos

- GNU Make
- Compilador C++ (compatible con C++11 o superior)
- Librerías estándar de C++

## Estructura del proyecto
```
├── Makefile            # Reglas de compilación y limpieza
├── README.md           # Documentación del proyecto
├── main.cpp            # Programa principal de pruebas y visualización
├── kruskal/            # Implementación de variantes de Kruskal
│   ├── kruskal.hpp     # Declaraciones de funciones de Kruskal
│   └── kruskal.cpp     # Definiciones de algoritmos
└── estructuras/        # Estructuras de datos auxiliares
    ├── puntos.hpp/.cpp # Clase Punto y utilidades de peso
    ├── aristas.hpp/.cpp# Estructura Arista y generación de listas/colas
    ├── union_find.hpp/.cpp # Estructura Union-Find
    └── grilla.cpp      # Opcional: visualización de la grilla
```

## Para ejecutar el experimento
Para realizar el experimento: 
1) Sencillamente se debe clonar el repositorio con `git clone <url de la repo>`.
2) Desde la raíz del directorio, simplemente ejecutar el comando ```make```.
3) Una vez compilado, se puede ejecutar el programa con: `./test_kruskal`. 
4) Para la visualización del algoritmo con: `./grilla`.

<<<<<<< HEAD
Si se quiere ver mas puntos dentro de la grilla, ir al archivo `grilla.cpp` y cambiar la linea comentada, esta predefinida en N=32.
=======
Desde la raíz del proyecto:

```bash
make      # Compila todos los binarios (test_kruskal y grilla)
```

### Ejecutar pruebas
```bash
./test_kruskal    # Corre casos de prueba automatizados y muestra tiempo de ejecución
```

### Visualizar algoritmo
```bash
./grilla          # Muestra animación simple de la construcción del MST en consola
```

## Comentarios y Documentación (Doxygen)

Para las firmas y documentación se utilizó la IA de Copilot.

## Ejemplo de uso

```bash
# Generar MST con heap y optimización
./test_kruskal --variant heap_opt --input datos/grafo.txt
```

---
*Fin de la documentación del proyecto.*
>>>>>>> 64c8a31 (Update readme and add documentation)
