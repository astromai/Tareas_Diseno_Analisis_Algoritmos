#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "estructuras/puntos.hpp"
#include "estructuras/aristas.hpp"
#include "kruskal/kruskal.hpp"
#include "estructuras/union_find.hpp"

using namespace std;

const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 700;
const int NUM_CELLS = 10;

void drawGrid(SDL_Renderer* renderer, int width, int height, int numCells) {
    int cellSize = width / numCells;

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // gris claro

    for (int i = 0; i <= numCells; ++i) {
        int x = i * cellSize;
        int y = i * cellSize;

        // Línea vertical
        SDL_RenderDrawLine(renderer, x, 0, x, height);

        // Línea horizontal
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }
}

void drawPoints(SDL_Renderer* renderer, const vector<Punto>& puntos) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // azul

    for (const Punto& p : puntos) {
        int px = static_cast<int>(p.getX() * WINDOW_WIDTH);
        int py = static_cast<int>((1.0 - p.getY()) * WINDOW_HEIGHT); // invertir eje Y

        SDL_Rect rect = { px - 5, py - 5, 10, 10 };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void drawEdges(SDL_Renderer* renderer, const std::vector<Arista>& aristas, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);  // rojo

    for (const Arista& a : aristas) {
        int x1 = static_cast<int>(a.p1->getX() * WINDOW_WIDTH);
        int y1 = static_cast<int>((1.0 - a.p1->getY()) * WINDOW_HEIGHT);

        int x2 = static_cast<int>(a.p2->getX() * WINDOW_WIDTH);
        int y2 = static_cast<int>((1.0 - a.p2->getY()) * WINDOW_HEIGHT);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error al iniciar SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Grilla 2D",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "No se pudo crear la ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "No se pudo crear el renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    vector<Punto> puntos(32); // Cambiar para ver mas N puntos en el plano

    vector<Punto*> ptrs;
    for (auto& p : puntos) {
        ptrs.push_back(&p);
    }

    vector<Arista> mst1 = kruskal_w_sort_opt(ptrs);
    vector<Arista> aristas = AristaUtils::getAristasSort(ptrs);
    
    bool running = true;
    bool mostrar_mst = false;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mostrar_mst = !mostrar_mst;  // cambia entre true y false
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // fondo blanco
        SDL_RenderClear(renderer);

        drawGrid(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, NUM_CELLS);
        drawPoints(renderer, puntos);
        if (mostrar_mst) {
            drawEdges(renderer, mst1,255,0,0);
        } else {
            drawEdges(renderer, aristas,0,60,255);        
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
