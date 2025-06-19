/** \file puntos.hpp
 *  \brief Definición de la clase Punto y funciones utilitarias para cálculo de peso de aristas.
 */

#ifndef PUNTOS_HPP
#define PUNTOS_HPP

#include <cstdint>
#include <utility>
#include <cmath>

using namespace std;

/** \class Punto
 *  \brief Representa un punto en el plano con enlace para conjuntos disjuntos.
 *
 *  La clase Punto almacena coordenadas (x,y) y un puntero padre para estructuras Union-Find.
 */
class Punto {
    private:
        double x;        ///< Coordenada X del punto
        double y;        ///< Coordenada Y del punto
        Punto* padre;    ///< Puntero al representante en Union-Find

    public:
        /**
         *  \brief Construye un punto vacío con coordenadas (0,0) y padre nullptr.
         */
        Punto();
        
        /** \brief Obtiene la coordenada X.
         *  \return Valor de la coordenada X.
         */
        double getX() const { return x; };

        /** \brief Obtiene la coordenada Y.
         *  \return Valor de la coordenada Y.
         */
        double getY() const { return y; };

        /**
         *  \brief Encuentra el representante del conjunto al que pertenece este punto.
         *
         *  Realiza búsqueda sin compresión de camino.
         *  \return Puntero al representante del conjunto.
         */
        Punto* find_rep();

        /**
         *  \brief Asigna un nuevo padre para la estructura Union-Find.
         *  \param nuevo_padre Puntero al elemento que se convertirá en padre.
         */
        void set_padre(Punto* nuevo_padre);
};

namespace PuntoUtils 
{   
    /**
     *  \brief Calcula el peso de la arista entre dos puntos.
     *
     *  El peso corresponde al cuadrado de la distancia euclidiana.
     *  \param p1 Primer punto.
     *  \param p2 Segundo punto.
     *  \return Peso (distancia al cuadrado).
     */
    double getWeight(const Punto& p1,const Punto& p2);
} // namespace PuntoUtils 

#endif