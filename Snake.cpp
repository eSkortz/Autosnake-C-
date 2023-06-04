#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

using namespace std;

bool gameOver;
bool gameWin;
const int width = 20;
const int height = 20;
int x, y;
int fruitX, fruitY;
int score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;
enum eDifficulty { EASY, MEDIUM, HARD };
eDifficulty difficulty;
enum eMode { MANUAL, AUTO };
eMode mode;

bool obstacles[width][height];

struct Point
{
    int x;
    int y;

    Point(int _x, int _y)
        : x(_x), y(_y)
    {
    }

    Point() : x(0), y(0)
    {
    }
};

bool IsTail(int posX, int posY)
{
    for (int i = 1; i < nTail; i++)
    {
        if (posX == tailX[i] && posY == tailY[i])
        {
            return true;
        }
    }
    return false;
}

void Setup()
{
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    score = 0;
    nTail = 0;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            obstacles[i][j] = false;
        }
    }

    do
    {
        fruitX = rand() % width;
        fruitY = rand() % height;
    } while (obstacles[fruitX][fruitY]);

    switch (difficulty)
    {
    case EASY:
        break;
    case MEDIUM:
        for (int i = 0; i < 5; i++)
        {
            int obstacleX = rand() % width;
            int obstacleY = rand() % height;
            obstacles[obstacleX][obstacleY] = true;
        }
        break;
    case HARD:
        for (int i = 0; i < 10; i++)
        {
            int obstacleX = rand() % width;
            int obstacleY = rand() % height;
            obstacles[obstacleX][obstacleY] = true;
        }
        break;
    }
}

void Draw()
{
    system("clear");

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                cout << "#";

            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "F";
            else if (obstacles[j][i])
                cout << "@";
            else
            {
                bool printTail = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        cout << "o";
                        printTail = true;
                    }
                }

                if (!printTail)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    cout << "Score:" << score << endl;
    cout << "Difficulty: ";
    switch (difficulty)
    {
    case EASY:
        cout << "Easy";
        break;
    case MEDIUM:
        cout << "Medium";
        break;
    case HARD:
        cout << "Hard";
        break;
    }
    cout << endl;
}

void Input()
{
    if (true)
    {
        switch (getchar())
        {
        case 'a':
            if (dir != RIGHT)
                dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT)
                dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN)
                dir = UP;
            break;
        case 's':
            if (dir != UP)
                dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    }

    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    if (obstacles[x][y])
        gameOver = true;

    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
        {
            gameOver = true;
            return;
        }
    }

    vector<pair<int, int>> snakeCoords;
    for (int i = 0; i < nTail; i++)
    {
        snakeCoords.push_back(make_pair(tailX[i], tailY[i]));
    }

    bool spawnInTail = true;

    if (x == fruitX && y == fruitY)
    {
        score += 10;
        do
        {
            while (spawnInTail)
            {
                spawnInTail = false;
                fruitX = rand() % width;
                fruitY = rand() % height;

                for (const auto& coord : snakeCoords)
                {
                    if (fruitX == coord.first && fruitY == coord.second)
                    {
                        spawnInTail = true;
                        break;
                    }
                }
            }
        } while (obstacles[fruitX][fruitY]);
        nTail++;
    }

    if (score >= 100)
    {
        gameWin = true;
    }
}

void AutoPlayGame(int fruitPosX, int fruitPosY)
{
    queue<Point> queue;
    vector<vector<bool>> visited(width, vector<bool>(height, false));
    vector<vector<Point>> parents(width, vector<Point>(height, Point()));
    vector<vector<bool>> obstacleMap(width, vector<bool>(height, false));

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            obstacleMap[i][j] = obstacles[i][j];
        }
    }

    Point start(x, y);
    Point target(fruitPosX, fruitPosY);

    queue.push(start);
    visited[start.x][start.y] = true;

    bool pathFound = false;

    while (!queue.empty())
    {
        Point current = queue.front();
        queue.pop();

        if (current.x == target.x && current.y == target.y && !IsTail(current.x, current.y))
        {
            pathFound = true;
            break;
        }

        vector<Point> neighbors;
        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };
        for (int k = 0; k < 4; ++k) {
            Point next(current.x + dx[k], current.y + dy[k]);
            if (next.x >= 0 && next.x < width && next.y >= 0 && next.y < height && !IsTail(next.x, next.y) && !visited[next.x][next.y] && !obstacleMap[next.x][next.y]) {
                queue.push(next);
                parents[next.x][next.y] = current;
                visited[next.x][next.y] = true;
            }
        }
    }

    if (pathFound)
    {
        vector <Point> path;
        Point current = target;
        while (current.x != x || current.y != y) {
            path.push_back(current);
            current = parents[current.x][current.y];
        }
        reverse(path.begin(), path.end());
        for (Point p : path) {
            int dx[] = { -1, 1, 0, 0 };
            int dy[] = { 0, 0, -1, 1 };
            eDirection dirs[] = { LEFT, RIGHT, UP, DOWN };
            for (int k = 0; k < 4; ++k) {
                if (x + dx[k] == p.x && y + dy[k] == p.y) {
                    dir = dirs[k];
                    break;
                }
            }
            Draw();
            Logic();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void GameLoop()
{
    while (!gameOver && !gameWin)
    {
        if (mode == AUTO)
            AutoPlayGame(fruitX, fruitY);
        else if (mode == MANUAL) {
            Draw();
            Input();
            Logic();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (gameWin)
    {
        cout << "Congratulations! You win!" << endl;
    }
    else
    {
        cout << "Game Over!" << endl;
    }
}

int main()
{
    cout << "Welcome to Snake!" << endl;

    cout << "Select difficulty level:" << endl;
    cout << "1. Easy" << endl;
    cout << "2. Medium" << endl;
    cout << "3. Hard" << endl;

    int difficultyChoice;
    cin >> difficultyChoice;

    switch (difficultyChoice)
    {
    case 1:
        difficulty = EASY;
        break;
    case 2:
        difficulty = MEDIUM;
        break;
    case 3:
        difficulty = HARD;
        break;
    }

    cout << "Select mode:" << endl;
    cout << "1. Manual" << endl;
    cout << "2. Auto" << endl;

    int modeChoice;
    cin >> modeChoice;

    switch (modeChoice)
    {
    case 1:
        mode = MANUAL;
        break;
    case 2:
        mode = AUTO;
        break;
    }

    srand(static_cast<unsigned>(time(0)));
    Setup();
    GameLoop();

    return 0;
}
