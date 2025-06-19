// ** \file aristas.hpp
//  \brief Definición de Arista y utilidades de generación de listas y heaps.

#ifndef ARISTAS_HPP
#define ARISTAS_HPP

#include "puntos.hpp"
#include <vector>
#include <algorithm>
#include <queue>

using namespace PuntoUtils;
using namespace std;

/**
 *  \struct Arista
 *  \brief Representa una arista entre dos puntos con un peso.
 *
 *  Contiene punteros a los extremos (p1,p2) y el peso calculado.
 */
struct Arista {
    Punto* p1;      ///< Primer extremo de la arista
    Punto* p2;      ///< Segundo extremo de la arista
    double peso;    ///< Peso de la arista (distancia al cuadrado)

    /**
     *  \brief Construye una arista calculando automáticamente el peso.
     *  \param a Puntero al primer punto.
     *  \param b Puntero al segundo punto.
     */
    Arista(Punto* a, Punto* b);
};

/**
 *  \struct ComparadorAristas
 *  \brief Comparador para priority_queue que crea un min-heap por peso.
 */
struct ComparadorAristas {
    bool operator()(const Arista& a, const Arista& b) const {
        return a.peso > b.peso; ///< Ordena inversamente para min-heap
    }
};

namespace AristaUtils
{
    /**
     *  \brief Genera y ordena la lista completa de aristas de un conjunto de puntos.
     *  \param puntos Vector de punteros a puntos.
     *  \return Vector de Arista ordenado por peso creciente.
     */
    vector<Arista> getAristasSort(const vector<Punto*>& puntos);

    /**
     *  \brief Construye un heap (priority_queue) con todas las aristas.
     *  \param puntos Vector de punteros a puntos.
     *  \return priority_queue de Arista con comparador de min-heap.
     */
    priority_queue<Arista, vector<Arista>, ComparadorAristas> construirHeapAristas(const vector<Punto*>& puntos);
}

#endif