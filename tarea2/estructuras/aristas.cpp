#include "aristas.hpp"


Arista::Arista(Punto* a, Punto* b)
    : p1(a), p2(b), peso(PuntoUtils::getWeight(*a, *b)) {}

namespace AristaUtils {

    auto comp = [](const Arista& a, const Arista& b) {
        return a.peso > b.peso;
    };

    vector<Arista> getAristasSort(const vector<Punto*>& puntos) {
        vector<Arista> aristas;
        for (size_t i = 0; i < puntos.size(); ++i) {
            for (size_t j = i + 1; j < puntos.size(); ++j) {
                aristas.emplace_back(puntos[i], puntos[j]);
            }
        }
        sort(aristas.begin(), aristas.end(), comp);
        return aristas;
    }

        // Esto se hace solo una vez, al inicio de Kruskal
    priority_queue<Arista, vector<Arista>, ComparadorAristas>
    construirHeapAristas(const vector<Punto*>& puntos) {
        priority_queue<Arista, vector<Arista>, ComparadorAristas> heap;
        for (size_t i = 0; i < puntos.size(); ++i) {
            for (size_t j = i + 1; j < puntos.size(); ++j) {
                heap.emplace(puntos[i], puntos[j]);
            }
        }
        return heap;
    }

}


