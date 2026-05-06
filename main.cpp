#include <Bits.h>
#ifdef _WIN32
#include "win/game.h"
#else
#include "linux/game.h"
#endif
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>

constexpr int width = 40, height = 20;
int x, y, fruitX, fruitY, score, record;
std::vector<std::pair<int,int>> snake;
enum Direction : int { STOP=0, LEFT, RIGHT, UP, DOWN } dir;
#define SPACE '.'
#define HEAD '@'
#define TAIL 'o'
#define BORDER '#'
#define APPLE 'Q'
#define FRUIT_SCORE 10
#define AI
#define LOOP

#ifdef AI
std::pair<int, int> getCoordsAfterMove(int curX, int curY, Direction d) {
    if (d == LEFT) curX--;
    else if (d == RIGHT) curX++;
    else if (d == UP) curY--;
    else if (d == DOWN) curY++;

    if (curX < 0) curX = width - 1;
    else if (curX >= width) curX = 0;
    if (curY < 0) curY = height - 1;
    else if (curY >= height) curY = 0;

    return {curX, curY};
}

bool isSafe(int nextX, int nextY) {
    if (nextX < 0) nextX = width - 1;
    else if (nextX >= width) nextX = 0;
    if (nextY < 0) nextY = height - 1;
    else if (nextY >= height) nextY = 0;

    for (auto const& segment : snake) {
        if (segment.first == nextX && segment.second == nextY) return false;
    }
    return true;
}

void SimpleAI() {
    int dx = fruitX - x;
    int dy = fruitY - y;

    if (abs(dx) > width / 2) dx = (dx > 0) ? dx - width : dx + width;
    if (abs(dy) > height / 2) dy = (dy > 0) ? dy - height : dy + height;

    auto scoreDirection = [&](Direction d) {
        // Проверка на разворот на 180 градусов (смерть)
        if ((d == LEFT && dir == RIGHT) || (d == RIGHT && dir == LEFT) ||
            (d == UP && dir == DOWN) || (d == DOWN && dir == UP)) return -100;

        auto next = getCoordsAfterMove(x, y, d);
        if (!isSafe(next.first, next.second)) return -200;

        int points = 0;
        if (d == LEFT && dx < 0) points++;
        if (d == RIGHT && dx > 0) points++;
        if (d == UP && dy < 0) points++;
        if (d == DOWN && dy > 0) points++;
        return points;
    };

    Direction bestDir = dir;
    int bestScore = -999;

    for (int d = 1; d < 5; d++) {
        int s = scoreDirection(static_cast<Direction>(d));
        if (s > bestScore) {
            bestScore = s;
            bestDir = static_cast<Direction>(d);
        }
    }

    dir = bestDir;
}
#endif

void Setup();
void PlaceFruit();

inline void DrawBorder() {
    setColor(rand() %10);
    setCursorPos(0,0);
    for (int i = 0; i < width + 2; i++) std::cout << BORDER;
    for (int j = 0; j < height; j++) {
        std::cout << '\n';
        std::cout << BORDER;
        for (int i = 0; i < width; i++) std::cout << SPACE;
        std::cout << BORDER;
    }
    std::cout << '\n';
    for (int i = 0; i < width + 2; i++) std::cout << BORDER;
}

inline void HandleInput() {
    if (!kbhit_wrapper()) return;
    int ch = getch_wrapper();
    switch (ch) {
        case 'a': case 'A': case 75: if (dir != RIGHT) dir = LEFT; break;
        case 'd': case 'D': case 77: if (dir != LEFT) dir = RIGHT; break;
        case 'w': case 'W': case 72: if (dir != DOWN) dir = UP; break;
        case 's': case 'S': case 80: if (dir != UP) dir = DOWN; break;
        case 'x': case 'X': dir = STOP; break;
    }
}

inline void BorderOutHandler() {
    if (x < 0) x = width - 1;
    else if (x >= width) x = 0;
    if (y < 0) y = height - 1;
    else if (y >= height) y = 0;
}

inline void DeadHandler() {
    setCursorPos(0, height + 2);
    std::cout << "Game Over! Final score: " << score << "/" << record << '\n';
#ifdef LOOP
    Setup();
#else
    exit(0);
#endif
}

inline void WinHandler() {
    #ifdef LOOP
        int r {score};
        Setup();
        score = r;
        record = (((record) > (score)) ? (record) : (score));
    #else
        exit(0);
    #endif
}

void Setup() {
    dir = STOP;
    x = width / 2;
    y = height / 2;
    snake.clear();
    snake.push_back({x, y});
    score = 0;
    srand(time(NULL));
    DrawBorder();
    PlaceFruit();
}

void PlaceFruit() {
    int totalCells = width * height;
    std::vector<int> freeIndices;
    freeIndices.reserve(totalCells);
    for (int i = 0; i < totalCells; ++i) freeIndices.push_back(i);
    for (auto const& segment : snake) {
        int occupiedIdx = segment.second * width + segment.first;
        freeIndices.erase(std::remove(freeIndices.begin(), freeIndices.end(), occupiedIdx), freeIndices.end());
    }
    
    if (freeIndices.empty()) {
        WinHandler();
        return;
    }

    int targetIdx = freeIndices[rand() % freeIndices.size()];

    fruitX = targetIdx % width;
    fruitY = targetIdx / width;

    setColor(4); 
    setCursorPos(fruitX + 1, fruitY + 1);
    std::cout << APPLE;
    setColor(0);
}

void MoveSnake() {
    if (dir == LEFT) x--;
    else if (dir == RIGHT) x++;
    else if (dir == UP) y--;
    else if (dir == DOWN) y++;

    BorderOutHandler(); 

    int oldHeadX = snake[0].first;
    int oldHeadY = snake[0].second;

    std::pair<int, int> tailToErase = snake.back();
    for (size_t i = snake.size() - 1; i > 0; i--) snake[i] = snake[i - 1];
    
    snake[0] = {x, y};

    setColor(2);
    setCursorPos(oldHeadX + 1, oldHeadY + 1);
    std::cout << TAIL;

    setCursorPos(x + 1, y + 1);
    std::cout << HEAD;

    if (x == fruitX && y == fruitY) {
        score += FRUIT_SCORE;
        snake.push_back(tailToErase);
        setCursorPos(0, height + 2);
#ifdef DEBUG
        std::cout << "Score: " << score << fruitX << " " << fruitY << " / " << x << ":" << y << std::flush;
#else
        setColor(15);
        std::cout << "Score: " << score << std::flush;
#endif
        PlaceFruit(); 
    } else {
        setColor(rand() %10);
        setCursorPos(tailToErase.first + 1, tailToErase.second + 1);
        std::cout << SPACE;
    }
}

void Logic() 
{   
    #ifdef AI
        SimpleAI();
    #else
        HandleInput();
    #endif
    MoveSnake();
    for (size_t i = 1; i < snake.size(); i++) {
        if (snake[i].first == x && snake[i].second == y) DeadHandler();
    }
}

int main() {
    initterminal();
    Setup();
    while (true) {
        Logic();
        sleep_ms(100);
    }
#ifdef _WIN32
    restoreterminal();
#endif
    return 0;
}