#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP

#include "puntos.hpp"
#include <unordered_map>
#include <map>

using namespace std;

class UnionFind {
private:
    unordered_map<Punto*, Punto*> padre;
    map<Punto*, int> tree_size;
    
public:
    void make_set(Punto* p);
    Punto* find_rep_opt(Punto* p);
    Punto* find_rep(Punto* p);
    void union_eq_class(Punto* a, Punto* b);
};

#endif