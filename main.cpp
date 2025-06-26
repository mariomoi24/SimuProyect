#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Event.hpp>
#include "json.hpp"

using namespace std;
using namespace sf;
using json = nlohmann::json;

const int rows = 10;
const int cols = 10;
map<pair<int, int>, int> paredesTemporales;
const int TURNOS_CAMBIO = 10;

class Jugador{
private:
    int jugadori,  jugadorj;
    int pasosRestantes, pasosDados;
    int itemsRecogidos = 0;
    RectangleShape jugador;

public:
    Jugador(int x, int y, int bateriaInicial = 50){
        jugadori = x;
        jugadorj = y;
        pasosRestantes = bateriaInicial;
        pasosDados = 0;
        jugador.setSize({48, 48});
        jugador.setPosition({y*50.f, x*50.f});
        jugador.setOutlineThickness(2.0f);
        jugador.setOutlineColor(Color::Black);
        jugador.setFillColor(Color::Blue);
    }

    void moverJugador(char direccion, string grid[][10]){
        if (pasosRestantes <= 0) return;
        bool seMovio = false;
        switch(direccion){
            case 'L':
                if(jugadorj-1 >= 0 && grid[jugadori][jugadorj-1] != "#"){
                    jugador.move({-50,0});
                    jugadorj--;
                    seMovio = true;
                }
                break;
            case 'R':
               if(grid[jugadori][jugadorj+1] != "#" && (jugadorj < cols-1)){
                    jugador.move({50,0});
                    jugadorj++;
                    seMovio = true;
                }
                break;
            case 'U':
                if(grid[jugadori-1][jugadorj] != "#" && (jugadori > 0)){
                    jugador.move({0,-50});
                    jugadori--;
                    seMovio = true;
                }
                break;
            case 'D':
                if(grid[jugadori+1][jugadorj] != "#" && (jugadori < rows-1)){
                    jugador.move({0,50});
                    jugadori++;
                    seMovio = true;
                }
                break;
        }
        if (seMovio) {
            pasosRestantes--;
            pasosDados++;
        }
    }

    RectangleShape getJugador(){ return jugador; }
    int getPosXJugador(){ return jugadori; }
    int getPosYJugador(){ return jugadorj; }
    int getBateria() { return pasosRestantes; }

    void recogerItem() { itemsRecogidos++; }
    int getItems() const { return itemsRecogidos; }

    void setPosicion(int i, int j) {
        jugadori = i;
        jugadorj = j;
        jugador.setPosition({j * 50.f, i * 50.f});
    }
};

// BFS para encontrar camino más corto, guarda en caminoX y caminoY el camino desde (sx, sy) a (gx, gy)
// Retorna true si existe camino, false si no.
bool bfsCamino(int sx, int sy, int gx, int gy, string grid[][10], vector<int>& caminoX, vector<int>& caminoY) {
    bool visitado[rows][cols];
    int padreX[rows][cols];
    int padreY[rows][cols];

    for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++) {
            visitado[i][j] = false;
            padreX[i][j] = -1;
            padreY[i][j] = -1;
        }

    queue<int> qx, qy;
    qx.push(sx);
    qy.push(sy);
    visitado[sx][sy] = true;

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    bool encontrado = false;

    while (!qx.empty()) {
        int x = qx.front(); qx.pop();
        int y = qy.front(); qy.pop();

        if (x == gx && y == gy) {
            encontrado = true;
            break;
        }

        for (int d=0; d<4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                if (!visitado[nx][ny] && grid[nx][ny] != "#") {
                    visitado[nx][ny] = true;
                    padreX[nx][ny] = x;
                    padreY[nx][ny] = y;
                    qx.push(nx);
                    qy.push(ny);
                }
            }
        }
    }

    if (!encontrado) return false;

    // Reconstruir camino desde meta hasta inicio
    int cx = gx;
    int cy = gy;
    vector<int> revX, revY;
    while (!(cx == sx && cy == sy)) {
        revX.push_back(cx);
        revY.push_back(cy);
        int px = padreX[cx][cy];
        int py = padreY[cx][cy];
        cx = px;
        cy = py;
    }
    revX.push_back(sx);
    revY.push_back(sy);

    // Invertir para camino correcto de inicio a meta
    for (int i = (int)revX.size()-1; i >= 0; i--) {
        caminoX.push_back(revX[i]);
        caminoY.push_back(revY[i]);
    }

    return true;
}

void inicioJuego(string grid[][10], int& jugadorx, int& jugadory, RectangleShape malla[][cols], int& virusFila, int& virusColumna, map<pair<int,int>, int>& paredesTemporales, int& metaX, int& metaY){
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            if(grid[i][j] == "#"){                                      //Pared
                malla[i][j].setPosition({(float)j*50.f, (float)i*50});  //Posicion de pared
                malla[i][j].setSize({50, 50});                          //Tamanio
                malla[i][j].setOutlineThickness(2.0f);                  //Grosor lineas divisoras
                malla[i][j].setOutlineColor(Color::Black);              //Color lineas divisoras
                malla[i][j].setFillColor(Color::Black);                 //Color solido para celda

                if (rand() % 100 < 15) {                                // 15% de paredes serán temporales
                    paredesTemporales[make_pair(i, j)] = 0;                      // empieza en turno 0
                }

            }else if(grid[i][j] == "S"){                                //Inicio partida jugador
                jugadorx = i;
                jugadory = j;

                malla[i][j].setPosition({(float)j*50.f, (float)i*50});         
                malla[i][j].setSize({50, 50});
                malla[i][j].setOutlineThickness(2.0f);             
                malla[i][j].setOutlineColor(Color::Black);
                malla[i][j].setFillColor(Color::White);

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
                malla[i][j].setFillColor(Color::Red);
                virusFila = i;
                virusColumna = j;
            }else if(grid[i][j] == "G"){                                                      //Meta: G
                malla[i][j].setPosition({(float)j*50.f, (float)i*50});         
                malla[i][j].setSize({50, 50});
                malla[i][j].setOutlineThickness(2.0f);             
                malla[i][j].setOutlineColor(Color::Black);
                malla[i][j].setFillColor(Color::White);
                metaX = i;
                metaY = j;
            }
        }
    }
}

int main() {
    RenderWindow window(VideoMode({500, 550}), "Laberinto infectado con virus", Style::Close | Style::Titlebar);

    ifstream archivoJson("laberinto.json");
    if (!archivoJson) {
        cout << "No se pudo abrir laberinto.json" << endl;
        return 1;
    }

    json dataLaberinto;
    archivoJson >> dataLaberinto;
    string grid[rows][cols];

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            grid[i][j] = dataLaberinto["grid"][i][j];

    RectangleShape malla[rows][cols];
    int x, y, virusFila, virusColumna, metaX, metaY;
    inicioJuego(grid, x, y, malla, virusFila, virusColumna, paredesTemporales, metaX, metaY);
    Jugador p(x, y);

    Clock relojVirus;
    float intervaloCambio = 1.5f;

    Font font("resources/arial.ttf");
    Text textoBateria(font), textoItems(font), mensajeFinal(font), textoMeta(font);

    textoBateria.setCharacterSize(18);
    textoBateria.setStyle(Text::Bold);

    textoItems.setCharacterSize(18);
    textoItems.setStyle(Text::Bold);
    textoItems.setFillColor(Color::Green);

    mensajeFinal.setCharacterSize(30);
    mensajeFinal.setStyle(Text::Bold);
    mensajeFinal.setFillColor(Color::Red);
    mensajeFinal.setPosition({150, 200});

    textoMeta.setCharacterSize(16);
    textoMeta.setStyle(Text::Bold);
    textoMeta.setFillColor(Color::Red);

    bool juegoTerminado = false;

    // Variables para animar y resolver el camino
    bool resolviendoCamino = false;
    vector<int> caminoX, caminoY;
    int pasoActual = 0;
    Clock relojAnimacion;

    while (window.isOpen()) {
        
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (!juegoTerminado && !resolviendoCamino && event->is<Event::KeyPressed>()) {
                auto key = event->getIf<Event::KeyPressed>()->code;

                if (key == Keyboard::Key::Left){    p.moverJugador('L', grid);}
                if (key == Keyboard::Key::Right){   p.moverJugador('R', grid);}
                if (key == Keyboard::Key::Up){      p.moverJugador('U', grid);}
                if (key == Keyboard::Key::Down){    p.moverJugador('D', grid);}
                if (key == Keyboard::Key::R) {
                    caminoX.clear();
                    caminoY.clear();
                    pasoActual = 0;
                    if(bfsCamino(p.getPosXJugador(), p.getPosYJugador(), metaX, metaY, grid, caminoX, caminoY))
                    {
                        resolviendoCamino = true;
                        relojAnimacion.restart();
                    }
                }
            }
        }

        if (!resolviendoCamino) {
            // Mutación virus
            if (relojVirus.getElapsedTime().asSeconds() >= intervaloCambio) {
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        int ni = virusFila + di;
                        int nj = virusColumna + dj;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                            if (grid[ni][nj] == "#" || grid[ni][nj] == ".") {
                                if (rand() % 100 < 25) {
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
                relojVirus.restart();
            }

            // Paredes temporales
            map<pair<int,int>, int>::iterator it = paredesTemporales.begin();
            while(it != paredesTemporales.end()) {
                it->second++;
                if (it->second >= TURNOS_CAMBIO) {
                    int i = it->first.first;
                    int j = it->first.second;
                    grid[i][j] = ".";
                    malla[i][j].setFillColor(Color::White);
                    it = paredesTemporales.erase(it);
                } else {
                    ++it;
                }
            }
        }

        // Animación de la solución paso a paso
        if (resolviendoCamino) {
            if (relojAnimacion.getElapsedTime().asSeconds() > 0.25f && pasoActual < (int)caminoX.size()) {
                int fila = caminoX[pasoActual];
                int col = caminoY[pasoActual];
                p.setPosicion(fila, col);
                pasoActual++;
                relojAnimacion.restart();
            } else if (pasoActual >= (int)caminoX.size()) {
                resolviendoCamino = false;
            }
        }

        int i = p.getPosXJugador();
        int j = p.getPosYJugador();

        // Recolectar item
        if (!resolviendoCamino && grid[i][j] == "K") {
            p.recogerItem();
            grid[i][j] = ".";
            malla[i][j].setFillColor(Color::White);
        }

        // Verifica si llegó a la meta o si se quedó sin batería
        if (!juegoTerminado) {
            if (grid[i][j] == "G") {
                juegoTerminado = true;
                mensajeFinal.setString("CONGRATULATIONS!!!");
                mensajeFinal.setFillColor(Color::Blue);
            } else if (p.getBateria() <= 0) {
                juegoTerminado = true;
                mensajeFinal.setString("GAME OVER");
                mensajeFinal.setFillColor(Color::Red);
            }
        }

        // Dibujar
        window.clear(Color::White);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                window.draw(malla[i][j]);

        window.draw(p.getJugador());

        textoBateria.setString("Bateria: " + to_string(p.getBateria()));
        textoBateria.setPosition({10, 510});
        textoBateria.setFillColor(p.getBateria() <= 0 ? Color::Red : Color::Blue);
        window.draw(textoBateria);

        textoItems.setString("Items: " + to_string(p.getItems()));
        textoItems.setPosition({150, 510});
        window.draw(textoItems);

        textoMeta.setString("Meta");
        textoMeta.setPosition({metaY * 50.f + 5.f, metaX * 50.f + 15.f});
        window.draw(textoMeta);

        if (juegoTerminado) {
            window.draw(mensajeFinal);
        }

        window.display();
    }

    return 0;
}
