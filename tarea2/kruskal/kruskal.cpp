#include "kruskal.hpp"

vector<Arista> kruskal_w_sort_opt(vector<Punto*>& puntos){
    UnionFind uf;
    for (Punto* p : puntos) {
        uf.make_set(p);
    }

    vector<Arista> aristas = AristaUtils::getAristasSort(puntos);

    vector<Arista> mst;

    for (const Arista& arista : aristas) {
        Punto* u = arista.p1;
        Punto* v = arista.p2;

        if (uf.find_rep_opt(u) != uf.find_rep_opt(v)) {
            uf.union_eq_class(u, v);
            mst.push_back(arista);

            if (mst.size() == puntos.size() - 1) {
                break;
            }
        }
    }

    return mst;
}

vector<Arista> kruskal_w_sort_no_opt(vector<Punto*>& puntos){
    UnionFind uf;
    for (Punto* p : puntos) {
        uf.make_set(p);
    }

    vector<Arista> aristas = AristaUtils::getAristasSort(puntos);

    vector<Arista> mst;

    for (const Arista& arista : aristas) {
        Punto* u = arista.p1;
        Punto* v = arista.p2;

        if (uf.find_rep(u) != uf.find_rep(v)) {
            uf.union_eq_class(u, v);
            mst.push_back(arista);

            if (mst.size() == puntos.size() - 1) {
                break;
            }
        }
    }

    return mst;
}

vector<Arista> kruskal_w_heap_opt(vector<Punto*>& puntos) {
    UnionFind uf;
    for (Punto* p : puntos) {
        uf.make_set(p);
    }

    priority_queue<Arista, vector<Arista>, ComparadorAristas> aristas = AristaUtils::construirHeapAristas(puntos);

    vector<Arista> mst;

    while (!aristas.empty()) {
        Arista arista = aristas.top();
        aristas.pop();

        Punto* u = arista.p1;
        Punto* v = arista.p2;

        if (uf.find_rep_opt(u) != uf.find_rep_opt(v)) {
            uf.union_eq_class(u, v);
            mst.push_back(arista);

            if (mst.size() == puntos.size() - 1) {
                break;
            }
        }
    }
    return mst;
}

vector<Arista> kruskal_w_heap_no_opt(vector<Punto*>& puntos) {
    UnionFind uf;
    for (Punto* p : puntos) {
        uf.make_set(p);
    }

    priority_queue<Arista, vector<Arista>, ComparadorAristas> aristas = AristaUtils::construirHeapAristas(puntos);

    vector<Arista> mst;

    while (!aristas.empty()) {
        Arista arista = aristas.top();
        aristas.pop();

        Punto* u = arista.p1;
        Punto* v = arista.p2;

        if (uf.find_rep(u) != uf.find_rep(v)) {
            uf.union_eq_class(u, v);
            mst.push_back(arista);

            if (mst.size() == puntos.size() - 1) {
                break;
            }
        }
    }
    return mst;
}

