#include "puntos.hpp"
#include <random>

Punto::Punto():
	x([] {
		static mt19937 gen(random_device{}());
		static uniform_real_distribution<double> dist(0.0, 1.0);
		return dist(gen);
	}()),
	y([] {
		static mt19937 gen(random_device{}());
		static uniform_real_distribution<double> dist(0.0, 1.0);
		return dist(gen);
	}()),
	padre(this) {}

Punto* Punto::find_rep() {
	if (padre != this) {
		padre = padre->find_rep();  // path compression
		return padre;
	}
	return padre;
}

void Punto::set_padre(Punto* nuevo_padre) {
    padre = nuevo_padre;
	}

namespace PuntoUtils {
	double getWeight(const Punto& p1,const Punto& p2) {
    	double x1 = p1.getX();
    	double x2 = p2.getX();
		double y1 = p1.getY();
		double y2 = p2.getY();
		return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
	}
}