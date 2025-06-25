#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "json.hpp"

using namespace std;
using namespace sf;
using json = nlohmann::json;

const int rows = 10;
const int cols = 10;

class Jugador{
    private:
    int jugadori,  jugadorj;
    RectangleShape jugador;
    public:
        Jugador(int x, int y){
            jugadori = x;
            jugadorj = y;
            jugador.setSize({48, 48});
            jugador.setPosition({y*50.f, x*50.f});         
            jugador.setOutlineThickness(2.0f);             
            jugador.setOutlineColor(Color::Black);
            jugador.setFillColor(Color::Magenta);
        }

    void moverJugador(char direccion, string grid[][10]){

        switch(direccion){
            case 'L':
                if(jugadorj-1 > 0 && grid[jugadori][jugadorj-1] != "#"){
                    jugador.move({-50,0});
                    jugadorj--;
                }
                break;
            case 'R':
               if(grid[jugadori][jugadorj+1] != "#" && (jugadorj < cols-1)){
                    jugador.move({50,0});
                    jugadorj++;
                }
                break;
            case 'U':
                if(grid[jugadori-1][jugadorj] != "#" && (jugadori > 0)){
                    jugador.move({0,-50});
                    jugadori--;
                }
                break;
            case 'D':
                if(grid[jugadori+1][jugadorj] != "#" && (jugadori < rows-1)){
                    jugador.move({0,50});
                    jugadori++;
                }
                break;
            default:
                break;

        }
    }
    RectangleShape getJugador(){
        return jugador;
    }

};

void inicioJuego(string grid[][10], int& jugadorx, int& jugadory, RectangleShape malla[][cols], int& virusFila, int& virusColumna);

int main(){
    RenderWindow window(VideoMode({600, 600}), "Laberinto infectado con virus", Style::Close | Style::Titlebar); //Creo ventana

    ifstream archivoJson("laberinto.json");
    if (!archivoJson) {
        cout << "No se pudo abrir laberinto.json" << endl;
        exit(1);
    }
    json dataLaberinto;
    archivoJson >> dataLaberinto;
    const int fila = dataLaberinto["width"];
    const int columna = dataLaberinto["height"];
    string grid[rows][cols];

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j] = dataLaberinto["grid"][i][j];
        }
    }

    RectangleShape malla[rows][cols];
    int x, y;
    int virusFila, virusColumna;
    inicioJuego(grid, x, y, malla, virusFila, virusColumna);
    Jugador p(x, y);

    Clock relojVirus;
    float intervaloCambio = 1.5f;                               // segundos entre mutaciones

    while (window.isOpen()){                                    
        while (const optional event = window.pollEvent()){       
            if (event->is<Event::Closed>()){
                window.close();
            }
            if(Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
                p.moverJugador('L', grid);
            }
            if(Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
                p.moverJugador('R', grid);
            }
            if(Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
                p.moverJugador('U', grid);
            }
            if(Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
                p.moverJugador('D', grid);
            } 

        }

        window.clear(Color::White);

        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                window.draw(malla[i][j]);
            }
        }

        window.draw(p.getJugador());
        window.display();

        if (relojVirus.getElapsedTime().asSeconds() >= intervaloCambio) {
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int ni = virusFila + di;
                    int nj = virusColumna + dj;

                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                        if (grid[ni][nj] == "#" || grid[ni][nj] == ".") {
                            if (rand() % 100 < 25) {                    // 25% de probabilidad por celda
                                if (grid[ni][nj] == "#") {
                                    grid[ni][nj] = ".";
                                    malla[ni][nj].setFillColor(Color::White);
                                } else {
                                    grid[ni][nj] = "#";
                                    malla[ni][nj].setFillColor(Color::Black);
                                }
                            }
                        }
                    }
                }
            }
            relojVirus.restart();                           // reiniciar el reloj solo si hizo cambios
        }

    }
    return 0;
}

void inicioJuego(string grid[][10], int& jugadorx, int& jugadory, RectangleShape malla[][cols], int& virusFila, int& virusColumna){
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            if(grid[i][j] == "#"){                                      //Pared
                malla[i][j].setPosition({(float)j*50.f, (float)i*50});  //Posicion de pared
                malla[i][j].setSize({50, 50});                          //Tamanio
                malla[i][j].setOutlineThickness(2.0f);                  //Grosor lineas divisoras
                malla[i][j].setOutlineColor(Color::Black);              //Color lineas divisoras
                malla[i][j].setFillColor(Color::Black);                 //Color solido para celda
            }else if(grid[i][j] == "S"){                                //Inicio partida jugador
                jugadorx = i;
                jugadory = j;

            }else if(grid[i][j] == "."){                                //Espacio
                malla[i][j].setPosition({static_cast<float>(j)*50.f, static_cast<float>(i)*50.f});
                malla[i][j].setSize({50, 50});
                malla[i][j].setOutlineThickness(2.0f);             
                malla[i][j].setOutlineColor(Color::Black);
                malla[i][j].setFillColor(Color::White);
            }else if(grid[i][j] == "K"){                                //Item
                malla[i][j].setPosition({static_cast<float>(j)*50.f, static_cast<float>(i)*50.f});
                malla[i][j].setSize({50, 50});
                malla[i][j].setOutlineThickness(2.0f);             
                malla[i][j].setOutlineColor(Color::Black);
                malla[i][j].setFillColor(Color::Green);

            }else if(grid[i][j] == "V"){                                 //Virus
                malla[i][j].setPosition({(float)j*50.f, (float)i*50});         
                malla[i][j].setSize({50, 50});
                malla[i][j].setOutlineThickness(2.0f);             
                malla[i][j].setOutlineColor(Color::Red);
                malla[i][j].setFillColor(Color::White);
                virusFila = i;
                virusColumna = j;
            }else{                                                      //Meta
                malla[i][j].setPosition({(float)j*50.f, (float)i*50});         
                malla[i][j].setSize({50, 50});
                malla[i][j].setOutlineThickness(2.0f);             
                malla[i][j].setOutlineColor(Color::Black);
                malla[i][j].setFillColor(Color::White);
            }
        }
    }

}

