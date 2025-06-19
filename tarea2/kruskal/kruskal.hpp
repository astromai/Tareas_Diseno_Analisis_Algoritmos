/** \file kruskal.hpp
 *  \brief Declaraciones de variantes del algoritmo de Kruskal.
 */
#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include "../estructuras/aristas.hpp"
#include <vector>
#include "../estructuras/union_find.hpp"

/**
 *  \brief Kruskal usando arreglo ordenado y optimización Union-Find.
 *  \param puntos Vector de punteros a puntos del grafo.
 *  \return Vector de aristas que forman el MST.
 */
vector<Arista> kruskal_w_sort_opt(vector<Punto*>& puntos);

/**
 *  \brief Kruskal usando arreglo ordenado sin optimización Union-Find.
 *  \param puntos Vector de punteros a puntos del grafo.
 *  \return Vector de aristas que forman el MST.
 */
vector<Arista> kruskal_w_sort_no_opt(vector<Punto*>& puntos);

/**
 *  \brief Kruskal usando heap y optimización Union-Find.
 *  \param puntos Vector de punteros a puntos del grafo.
 *  \return Vector de aristas que forman el MST.
 */
vector<Arista> kruskal_w_heap_opt(vector<Punto*>& puntos);

/**
 *  \brief Kruskal usando heap sin optimización Union-Find.
 *  \param puntos Vector de punteros a puntos del grafo.
 *  \return Vector de aristas que forman el MST.
 */
vector<Arista> kruskal_w_heap_no_opt(vector<Punto*>& puntos);

#endif