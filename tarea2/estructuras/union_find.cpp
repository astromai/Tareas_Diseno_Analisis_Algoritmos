#include "union_find.hpp"

void UnionFind::make_set(Punto* p) {
    padre[p] = p;
    tree_size[p] = 1;
}

Punto* UnionFind::find_rep(Punto* p) {
    if (padre[p] == p) {
        return p;
    }
    return find_rep(padre[p]);
}

Punto* UnionFind::find_rep_opt(Punto* p) {
    if (padre[p] != p) {
        padre[p] = find_rep(padre[p]);  // path compression
    }
    return padre[p];
}

void UnionFind::union_eq_class(Punto* a, Punto* b) {
    Punto* ra = find_rep(a);
    Punto* rb = find_rep(b);
    if (ra != rb) {
        if (tree_size[ra] < tree_size[rb]) {
            padre[ra] = rb;
            tree_size[rb] += tree_size[ra];  // <- ¡actualiza tamaño!
        } else {
            padre[rb] = ra;
            tree_size[ra] += tree_size[rb];  // <- ¡actualiza tamaño!
        }
    }
}

