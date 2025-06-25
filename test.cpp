#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <bits/stdc++.h>

using namespace std;
using namespace sf;
using json = nlohmann::json;

constexpr int TILE_SIZE = 50;

enum class CellType {
    Wall,
    Empty,
    Start,
    Goal,
    Collectible,
    Virus
};

struct Cell {
    CellType type;
    bool mutated = false;
};

struct Position {
    int x, y;
    bool operator==(const Position &other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position &other) const { return !(*this == other); }
};

class Game {
    public:
        Game(const string &path);
        void run();

    private:
        void loadMap(const string &path);
        void input();
        void showText();
        void render();
        bool inBounds(int x, int y);
        bool isWalkable(int x, int y);
        bool dfs(Position current, vector<vector<bool>> &visited);
        void mutateVirus();

        RenderWindow window;
        vector<vector<Cell>> grid;
        vector<Position> path;
        Position start, goal, player;
        int width, height;
        int battery;
        int turnCounter = 0;
        int walkableTilesCount = 0;
        vector<Position> virusCells;
        mt19937 rng{random_device{}()};
        
};

Game::Game(const string &path) {
    /* window.create(VideoMode(800, 800), "Escape the Grid", Style::Default);
    window window(VideoMode({800, 600}), "SFML window"); */
    window.create(VideoMode({800, 800}), "Escape the Grid", Style::Default);

    window.setFramerateLimit(60);
    
    loadMap(path);
}

void Game::loadMap(const string &path) {
    ifstream file(path);
    json jsonFile;
    file >> jsonFile;
    width = jsonFile["width"];
    height = jsonFile["height"];
    grid.resize(height, vector<Cell>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char symbol = jsonFile["grid"][y][x].get<string>()[0];
            Cell &cell = grid[y][x];
            switch (symbol) {
                case 'S':
                    cell.type = CellType::Start;
                    start = {x, y};
                    player = start;
                    break;
                case 'G':
                    cell.type = CellType::Goal;
                    goal = {x, y};
                    break;
                case '#':
                    cell.type = CellType::Wall;
                    break;
                case 'K':
                    cell.type = CellType::Collectible;
                    walkableTilesCount++;
                    break;
                case '.':
                    cell.type = CellType::Empty;
                    walkableTilesCount++;
                    break;
            }
        }
    }

    vector<vector<bool>> visited(height, vector<bool>(width, false));
    dfs(start, visited);

    uniform_int_distribution<int> dis(path.size(), walkableTilesCount * 2);
    battery = dis(rng); // Batería tiene tamaño random entre el camino mas corto y el doble de las celdas transitables

    int virusCount = (width * height) * 0.05; // Se eligio arbitrariamente el 5% del total de celdas
    uniform_int_distribution<int> xdist(0, width - 1);
    uniform_int_distribution<int> ydist(0, height - 1);
    while (virusCells.size() < virusCount) {
        int x = xdist(rng), y = ydist(rng);
        if ((grid[y][x].type == CellType::Empty || grid[y][x].type == CellType::Wall ) && find(virusCells.begin(), virusCells.end(), Position{x, y}) == virusCells.end()) {
            grid[y][x].type = CellType::Virus;
            virusCells.push_back({x, y});
        }
    }
}

bool Game::dfs(Position current, vector<vector<bool>> &visited) { // El propio algoritmo dfs
    if (!inBounds(current.x, current.y) || visited[current.y][current.x] || grid[current.y][current.x].type == CellType::Wall) {
        return false;
    }
    visited[current.y][current.x] = true;
    path.push_back(current);
    if (current == goal){
        return true;
    }
    vector<Position> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (auto &d : directions) {
        Position next = {current.x + d.x, current.y + d.y};
        if (dfs(next, visited)) {
            return true;
        }
    }
    path.pop_back();
    return false;
}

bool Game::inBounds(int x, int y) { // Verifica si las coordenadas están dentro de los límites del laberinto
    return x >= 0 && y >= 0 && x < width && y < height;
}

bool Game::isWalkable(int x, int y) {
    return inBounds(x, y) && (grid[y][x].type == CellType::Empty || grid[y][x].type == CellType::Goal || grid[y][x].type == CellType::Collectible);
}

void Game::input() { // Maneja la entrada del usuario
    if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
        window.close();
    }
    Position next = player;
    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
        next.y--;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
        next.y++;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
        next.x--;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
        next.x++;
    }
    if (next != player && isWalkable(next.x, next.y)) {
        player = next;
        battery--;
        turnCounter++;
        mutateVirus();
    }
}

void Game::mutateVirus() {
    uniform_int_distribution<int> mutChance(0, 100);
    for (auto &v : virusCells) {
        if (mutChance(rng) < 25) { // 25% chance per turn per virus
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = v.x + dx, ny = v.y + dy;
                    if (inBounds(nx, ny) && !(dx == 0 && dy == 0)) {
                        Cell &c = grid[ny][nx];
                        if (c.type == CellType::Wall) {
                            c.type = CellType::Empty;
                        }
                        else if (c.type == CellType::Empty) {
                            c.type = CellType::Wall;
                        }
                    }
                }
            }
        }
    }
}

void Game::showText() { // Muestra mensajes de gane o perdida
    //! añadir otra fuente
    const sf::Font font("arial.ttf");
    Text win(font, "Ganaste!", 50);
    Text lose(font, "Perdiste! Se agotó la batería", 50);
    if (player == goal) {
        window.draw(win);
        win.setFillColor(Color::Green);
        window.display();
        window.close();
    }
    if (battery <= 0) {
        window.draw(lose);
        win.setFillColor(Color::Red);
        window.display();
        window.close();
    }
}

void Game::render() {
    window.clear();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RectangleShape rect(Vector2f(TILE_SIZE - 1, TILE_SIZE - 1));
            rect.setPosition({x * TILE_SIZE, y * TILE_SIZE});
            Cell &cell = grid[y][x];
            switch (cell.type) {
                case CellType::Wall:
                    rect.setFillColor(Color::Black);
                    break;
                case CellType::Empty:
                    rect.setFillColor(Color::White);
                    break;
                case CellType::Start:
                    rect.setFillColor(Color::Green);
                    break;
                case CellType::Goal:
                    rect.setFillColor(Color::Red);
                    break;
                case CellType::Collectible:
                    rect.setFillColor(Color::Yellow);
                    break;
                case CellType::Virus:
                    rect.setFillColor(Color::Magenta);
                    break;
            }
            window.draw(rect);
        }
    }

    CircleShape playerShape(TILE_SIZE / 2 - 5);
    playerShape.setFillColor(Color::Blue);
    playerShape.setPosition({player.x * TILE_SIZE + 5, player.y * TILE_SIZE + 5});
    window.draw(playerShape);

    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        while (const optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<Event::Closed>())
                window.close();
        }
        input();
        showText();
        render();
    }
}

int main()
{
    Game game("laberintos\\laberinto10X10.json");
    game.run();
    return 0;
}
