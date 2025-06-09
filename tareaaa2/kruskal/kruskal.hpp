#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include "../estructuras/aristas.hpp"
#include <vector>
#include "../estructuras/union_find.hpp"

// • Algoritmo de Kruskal con la optimización de find y usando un arreglo de aristas ordenado.
vector<Arista> kruskal_w_sort_opt(vector<Punto*>& puntos);

// • Algoritmo de Kruskal sin la optimización de find y usando un heap clásico.
vector<Arista> kruskal_w_sort_no_opt(vector<Punto*>& puntos);

// • Algoritmo de Kruskal con la optimización de find y usando un heap clásico.
vector<Arista> kruskal_w_heap_opt(vector<Punto*>& puntos);

// • Algoritmo de Kruskal sin la optimización de find y usando un arreglo de aristas ordenado.
vector<Arista> kruskal_w_heap_no_opt(vector<Punto*>& puntos);

#endif