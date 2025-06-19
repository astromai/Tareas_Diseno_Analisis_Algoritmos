/** \file union_find.hpp
 *  \brief Interfaz de Union-Find con path compression y union by size.
 */
#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP

#include "puntos.hpp"
#include <unordered_map>
#include <map>

using namespace std;

/** \class UnionFind
 *  \brief Estructura de Conjuntos Disjuntos para detección de ciclos.
 *
 *  Soporta operaciones make_set, find_rep (con y sin optimización) y union_eq_class.
 */
class UnionFind {
private:
    unordered_map<Punto*, Punto*> padre; ///< Mapa de padre de cada elemento
    map<Punto*, int> tree_size;         ///< Tamaño de cada árbol (para union by size)
    
public:
    /**
     *  \brief Inicializa un nuevo conjunto con un solo elemento.
     *  \param p Puntero al punto a inicializar.
     */
    void make_set(Punto* p);

    /**
     *  \brief Encuentra el representante del conjunto con path compression.
     *  \param p Puntero al punto de consulta.
     *  \return Puntero al representante del conjunto.
     */
    Punto* find_rep_opt(Punto* p);

    /**
     *  \brief Encuentra el representante del conjunto sin optimización.
     *  \param p Puntero al punto de consulta.
     *  \return Puntero al representante del conjunto.
     */
    Punto* find_rep(Punto* p);

    /**
     *  \brief Une dos conjuntos por tamaño para evitar desequilibrios.
     *  \param a Primer representante o elemento del conjunto.
     *  \param b Segundo representante o elemento del conjunto.
     */
    void union_eq_class(Punto* a, Punto* b);
};

#endif