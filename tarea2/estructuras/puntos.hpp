#ifndef PUNTOS_HPP
#define PUNTOS_HPP

#include <cstdint>
#include <utility>
#include <cmath>

using namespace std;

class Punto {
    private:
        double x;
        double y;
        Punto* padre;

    public:
        Punto();
        
            double getX() const { return x; };
            double getY() const { return y; };

            Punto* find_rep();

            void set_padre(Punto* nuevo_padre);
};

namespace PuntoUtils 
{
    // el peso de la arista entre 2 nodos:
    // el cuadrado de la distancia euclidiana que hay entre ellos
    double getWeight(const Punto& p1,const Punto& p2);
} // namespace PuntoUtils 

#endif