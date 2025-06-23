#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "json.hpp"  // Asegúrate de tener esta biblioteca
#include <cstdlib>            // Para system()

using json = nlohmann::json;
using namespace std;

struct Laberinto {
    int width;
    int height;
    vector<vector<char>> grid;
};

Laberinto leerLaberintoDesdeJSON(const string& rutaArchivo) {
    Laberinto laberinto;
    ifstream archivo(rutaArchivo);
    
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + rutaArchivo);
    }

    json j;
    archivo >> j;

    laberinto.width = j["width"];
    laberinto.height = j["height"];
    laberinto.grid.resize(laberinto.height, vector<char>(laberinto.width));

    const auto& matriz = j["grid"];
    for (int i = 0; i < laberinto.height; ++i) {
        for (int jdx = 0; jdx < laberinto.width; ++jdx) {
            laberinto.grid[i][jdx] = matriz[i][jdx].get<string>()[0];
        }
    }

    return laberinto;
}

void imprimirLaberinto(const Laberinto& lab) {
    for (const auto& fila : lab.grid) {
        for (const auto& celda : fila) {
            cout << celda << ' ';
        }
        cout << '\n';
    }
}

int main() {
    try {
        string archivo = "modelosdelaberitos/laberinto10X10.json";  // Ajusta ruta si está en subcarpeta
        Laberinto lab = leerLaberintoDesdeJSON(archivo);
        cout << "Laberinto cargado (" << lab.width << "x" << lab.height << "):\n\n";
        imprimirLaberinto(lab);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
    }

    cout << "\nPresiona Enter para salir...";
    cin.get();  // Pausa para que puedas ver la salida

    return 0;
}
