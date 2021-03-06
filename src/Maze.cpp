#include <iostream>
#include <queue>
#include <stack>
#include <random>
#include "Maze.h"
#include "constants.h"

Maze::Maze() {
    createMaze();
    generateGraph();
    makeTiles();
}

int Maze::getIdx(int x, int y, vector<pair<int, pair<int, int> > > &cells) {
    for (auto &i : cells) {
        if (i.second.first == x && i.second.second == y)
            return i.first;
    }
    cout << "getIdx() couldn't find the index!" << endl;
    return -1;
}

void Maze::createMaze() {

    int m = MAZE_HEIGHT, n = MAZE_WIDTH;
    M = 2 * m + 1;
    N = 2 * n + 1;

    char maze[M][N];

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            maze[i][j] = (!(i & 1) || !(j & 1)) ? '#' : ' ';
        }
    }

    vector<pair<int, pair<int, int> > > gridCells;
    stack<pair<int, pair<int, int> >> st;
    vector<bool> visited(m * n, false);
    random_device rdev;
    mt19937 rng(rdev());
    uniform_int_distribution<mt19937::result_type> dist100(1, 100);

    for (int k = 0, i = 1; i < M; i += 2) {
        for (int j = 1; j < N; j += 2, k++) {
            gridCells.emplace_back(k, make_pair(i, j));
        }
    }

    int randIdx = (int) (dist100(rng) % m) * n;
    int nVisited = 1;
    st.push(gridCells[randIdx]);
    visited[randIdx] = true;

    while (nVisited < m * n) {

        vector<int> neighbours;
        int x = st.top().second.first;
        int y = st.top().second.second;

        // North
        if (x > 1 and maze[x - 2][y + 0] && !visited[getIdx(x - 2, y + 0, gridCells)]) {
            neighbours.push_back(0);
        }
        // East
        if (y < N - 2 and maze[x + 0][y + 2] && !visited[getIdx(x + 0, y + 2, gridCells)]) {
            neighbours.push_back(1);
        }
        // South
        if (x < M - 2 and maze[x + 2][y + 0] && !visited[getIdx(x + 2, y + 0, gridCells)]) {
            neighbours.push_back(2);
        }
        // West
        if (y > 1 and maze[x + 0][y - 2] && !visited[getIdx(x + 0, y - 2, gridCells)]) {
            neighbours.push_back(3);
        }
        // Neighbours available?
        if (!neighbours.empty()) {
            // Choosing a random direction and making a path between this cell and neighbour
            switch (neighbours[dist100(rng) % neighbours.size()]) {
                case 0: // North
                    maze[x - 1][y + 0] = ' ';
                    st.push(gridCells[getIdx(x - 2, y + 0, gridCells)]);
                    break;
                case 1: // East
                    maze[x + 0][y + 1] = ' ';
                    st.push(gridCells[getIdx(x + 0, y + 2, gridCells)]);
                    break;
                case 2: // South
                    maze[x + 1][y + 0] = ' ';
                    st.push(gridCells[getIdx(x + 2, y + 0, gridCells)]);
                    break;
                case 3: // West
                    maze[x + 0][y - 1] = ' ';
                    st.push(gridCells[getIdx(x + 0, y - 2, gridCells)]);
                default:
                    break;
            }

            visited[st.top().first] = true;
            nVisited++;
            continue;
        }
        st.pop();
    }

    for (int i = 1; i < M - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            if (maze[i][j] == '#') {
                if (dist100(rng) % 100 < 17) {
                    if (maze[i][j + 1] == '#' and maze[i + 1][j] == '#') continue;
                    if (maze[i][j + 1] == '#' and maze[i - 1][j] == '#') continue;
                    if (maze[i][j - 1] == '#' and maze[i + 1][j] == '#') continue;
                    if (maze[i][j - 1] == '#' and maze[i - 1][j] == '#') continue;
                    maze[i][j] = ' ';
                }
            }
        }
    }

    maze[0][1] = 'S';
    maze[2 * m][2 * n - 1] = 'E';

    for (int i = 0; i < M; i++) {
        string s;
        for (int j = 0; j < N; j++) {
            s += maze[i][j];
        }
        pat.push_back(s);
    }
}

void Maze::makeTiles() {
    glm::vec2 pos = glm::vec2(0.0f, 00.0f);

    for (auto &i : pat) {
        pos.x = 0.0f;
        for (char tileType : i) {
            auto *tile = new Tile(tileType == '#' ? "skeld-wall" : "stone-tile", pos, TILE_SIZE, tileType == '#');
            tiles.push_back(tile);
            pos.x += TILE_SIZE.x;
        }
        pos.y += TILE_SIZE.y;
    }
}

void Maze::generateGraph() {
    adj.resize(M * N);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int id = N * i + j;
            if (pat[i][j] != '#') {
                if (j < N - 1 and pat[i][j + 1] != '#') {
                    adj[id].push_back(N * i + j + 1);
                }
                if (j > 0 and pat[i][j - 1] != '#') {
                    adj[id].push_back(N * i + j - 1);
                }
                if (i < M - 1 and pat[i + 1][j] != '#') {
                    adj[id].push_back(N * (i + 1) + j);
                }
                if (i > 0 and pat[i - 1][j] != '#') {
                    adj[id].push_back(N * (i - 1) + j);
                }
            }
        }
    }
}

glm::vec4 Maze::changeColor(glm::vec2 pos, glm::vec3 color) {
    int id = getIdFromPos(pos);
    auto mini = glm::vec2(100000.0f, 100000.0f), maxi = glm::vec2(-300.0f, -300.0f);
    int x = id / N, y = id % N;
    for (int i = max(0, x - 1); i <= min(M - 1, x + 1); i++) {
        for (int j = max(0, y - 1); j <= min(y + 1, N - 1); j++) {
            tiles[i * N + j]->setColor(color);
            mini = min(mini, tiles[i * N + j]->getPosition());
            maxi = max(maxi, tiles[i * N + j]->getPosition() + TILE_SIZE);
        }
    }
    return glm::vec4(mini, maxi);
}

char Maze::runDjkstra(glm::vec2 destination, glm::vec2 begin) {

    int idDestination = verifyDestination(destination);
    int idBegin = getIdFromPos(begin);

    vector<int> d(M * N, 1e7), par(M * N, -1);
    d[idBegin] = 0;

    priority_queue<pair<int, int>> pq;
    pq.push({0, idBegin});

    while (!pq.empty()) {
        auto v = pq.top().second;
        pq.pop();
        for (auto u : adj[v]) {
            if (d[u] > d[v] + 1) {
                d[u] = d[v] + 1;
                par[u] = v;
                pq.push({-d[u], u});
            }
        }
    }

    int nextId;
    for (nextId = idDestination; par[nextId] != idBegin and par[nextId] != -1; nextId = par[nextId]);
    return getDirection(idBegin, nextId);
}

glm::vec2 Maze::getImposterPos() {
    int index = N - 2;
    for (int i = 1; i < M; i++) {
        if (pat[i][index] != '#') {
            return tiles[i * N + index]->getCenter();
        }
    }
    return glm::vec2(0.0f, 0.0f);
}

int Maze::getIdFromPos(glm::vec2 pos) const {

    int j = pos.x / TILE_SIZE.x;
    int i = pos.y / TILE_SIZE.y;
    return N * i + j;
}

char Maze::getDirection(int beginId, int nextId) const {

    pair<int, int> cellBegin = {beginId / N, beginId % N};
    pair<int, int> cellNext = {nextId / N, nextId % N};

    if (cellNext.first < cellBegin.first) return 'U';
    else if (cellNext.first > cellBegin.first) return 'D';
    else if (cellNext.second < cellBegin.second) return 'L';
    else if (cellNext.second > cellBegin.second) return 'R';
    else return 'I';
}

glm::vec2 Maze::getTaskTilePosition(int k) {

    int x = k == 1 ? 1 : (M - 4);
    int y = k == 1 ? (N - 4) : 1;

    for (int i = x; i < x + 3; i++) {
        for (int j = y; j < y + 3; j++) {
            if (pat[i][j] == ' ') {
                cout << tiles[i * N + j]->getCenter().x << " " << tiles[i * N + j]->getCenter().y << " #\n";
                return tiles[i * N + j]->getCenter();
            }
        }
    }
    return glm::vec2(0);
}

int Maze::verifyDestination(glm::vec2 destination) {
    int id = getIdFromPos(destination);
    int i = id / N, j = id % N;

    if (pat[i][j] == '#') {
        if (j < N - 1 and pat[i][j + 1] != '#') {
            j += 1;
        } else if (j > 0 and pat[i][j - 1] != '#') {
            j -= 1;
        }
        if (i < M - 1 and pat[i + 1][j] != '#') {
            i += 1;
        }
        if (i > 0 and pat[i - 1][j] != '#') {
            i -= 1;
        }
    }
    return N * i + j;
}

void Maze::makeObstacles() {
    random_device rdev;
    mt19937 rng(rdev());
    uniform_int_distribution<mt19937::result_type> dist100(1, 100);

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (pat[i][j] == ' ' and dist100(rng) % 100 < 10) {
                PowerUp *pup;
                if (dist100(rng) % 3 < 1) {
                    pup = new PowerUp("bomb", tiles[i * N + j]->getPosition(), TILE_SIZE / 1.3f, BOMB);
                } else {
                    pup = new PowerUp("coin", tiles[i * N + j]->getPosition(), TILE_SIZE / 1.3f, COIN);
                }
                pup->setCenter(tiles[i * N + j]->getCenter());
                obstacles.push_back(pup);
            }
        }
    }
}
