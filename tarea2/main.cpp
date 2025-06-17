#include <iostream>
#include "estructuras/puntos.hpp"
#include "estructuras/aristas.hpp"
#include <array>
#include <vector>
#include <chrono>
#include "kruskal/kruskal.hpp"
#include <numeric>
#include <fstream>
#include <filesystem>
#include <regex>

using namespace std;
using namespace AristaUtils;

vector<Punto*> punterosDesdeVector(const vector<Punto>& puntos) {
    vector<Punto*> ptrs;
    ptrs.reserve(puntos.size());
    for (const auto& p : puntos) {
        ptrs.push_back(const_cast<Punto*>(&p));
    }
    return ptrs;
}

// Funcion auxiliar
string generarNombreArchivo(const string& carpeta) {
    int maxN = 0;
    regex archivoRegex("res(\\d+)\\.csv");
    for (const auto& entry : filesystem::directory_iterator(carpeta)) {
        smatch match;
        string filename = entry.path().filename().string();
        if (regex_match(filename, match, archivoRegex)) {
            maxN = max(maxN, stoi(match[1]));
        }
    }
    return carpeta + "/res" + to_string(maxN + 1) + ".csv";
}

int main() {

    cout << '\n' <<"Test para probrar eficiencia del Algoritmo de Kruskal en distintos contextos. "  << '\n' << endl;

    const array<int, 8> N = {32,64,128,256,512,1024,2048,4096}; // Sizes
    vector<tuple<int, double, double, double, double>> resultados; 

    for (int size : N) {
        double suma_time_sort_opt = 0;
        double suma_time_heap_opt = 0;
        double suma_time_sort_no_opt = 0;
        double suma_time_heap_no_opt = 0;

        cout << "Iterando ... " << endl;
        
        for (int i= 1; i<=5; ++i) { // 5 generaciones
            
            vector<Punto> puntos(size); // crea 'size' puntos aleatorios
            vector<Punto*> puntos_ptrs = punterosDesdeVector(puntos);
                
            // kruskal con sort [opt]
            auto inicio1 = chrono::high_resolution_clock::now();
            vector<Arista> mst1 = kruskal_w_sort_opt(puntos_ptrs);
            auto fin1 = chrono::high_resolution_clock::now();
            chrono::duration<double> duracion1 = fin1 - inicio1;
            double time1 = duracion1.count();
            suma_time_sort_opt += time1;
            
            // kruskal con heap [opt]
            auto inicio2 = chrono::high_resolution_clock::now();
            vector<Arista> mst2 = kruskal_w_heap_opt(puntos_ptrs);
            auto fin2 = chrono::high_resolution_clock::now();
            chrono::duration<double> duracion2 = fin2 - inicio2;
            double time2 = duracion2.count();
            suma_time_heap_opt += time2;

            // kruskal con sort [no-opt]
            auto inicio3 = chrono::high_resolution_clock::now();
            vector<Arista> mst3 = kruskal_w_sort_no_opt(puntos_ptrs);
            auto fin3 = chrono::high_resolution_clock::now();
            chrono::duration<double> duracion3 = fin3 - inicio3;
            double time3 = duracion3.count();
            suma_time_sort_no_opt += time3;

            // kruskal con heap [no-opt]
            auto inicio4 = chrono::high_resolution_clock::now();
            vector<Arista> mst4 = kruskal_w_heap_no_opt(puntos_ptrs);
            auto fin4 = chrono::high_resolution_clock::now();
            chrono::duration<double> duracion4 = fin4 - inicio4;
            double time4 = duracion4.count();
            suma_time_heap_no_opt += time4;
        }

        cout << "Finalizada la iteración" << '\n' << endl;

        double avg1 = suma_time_sort_opt / 5.0;
        double avg2 = suma_time_heap_opt / 5.0;
        double avg3 = suma_time_sort_no_opt / 5.0;
        double avg4 = suma_time_heap_no_opt / 5.0;

        resultados.emplace_back(size, avg1, avg2, avg3, avg4);

        cout << "Resultados promedio para " << size << " puntos: " << '\n' <<endl;
        cout << "   Algoritmo de Kruskal con Sort (con optimización de find): " << avg1 << endl;
        cout << "   Algoritmo de Kruskal con Heap (con optimización de find): " << avg2 << endl;
        cout << "   Algoritmo de Kruskal con Sort (sin optimización de find): " << avg3 << endl;
        cout << "   Algoritmo de Kruskal con Heap (sin optimización de find): " << avg4 << endl;        
    
        cout << '\n' << endl;
    }

    cout << "Finalizado el programa." << endl;

    // guarda csv
    filesystem::create_directories("build/resultado");
    string nombreArchivo = generarNombreArchivo("build/resultado");

    ofstream archivo(nombreArchivo);
    archivo << "N,KruskalSortOpt,KruskalHeapOpt,KruskalSortNoOpt,KruskalHeapNoOpt\n";
    for (const auto& [n, t1, t2, t3, t4] : resultados) {
        archivo << n << "," << t1 << "," << t2 << "," << t3 << "," << t4 << "\n";
    }
    archivo.close();

    cout << "Resultados exportados a: " << nombreArchivo << endl;
}
