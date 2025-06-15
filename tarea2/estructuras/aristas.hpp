#ifndef ARISTAS_HPP
#define ARISTAS_HPP

#include "puntos.hpp"
#include <vector>
#include <algorithm>
#include <queue>

using namespace PuntoUtils;
using namespace std;

struct Arista {
    Punto* p1;
    Punto* p2;
    double peso;

    Arista(Punto* a, Punto* b);

};

struct ComparadorAristas {
    bool operator()(const Arista& a, const Arista& b) const {
        return a.peso > b.peso; // min-heap
    }
};

namespace AristaUtils
{
    vector<Arista> getAristasSort(const vector<Punto*>& puntos);
    priority_queue<Arista, vector<Arista>, ComparadorAristas> construirHeapAristas(const vector<Punto*>& puntos);
}

#endif