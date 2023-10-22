#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <conio.h>
#include <fstream>
#include <algorithm>
#include <cstring>

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define wall "\u2588"
#define pac "\u15E7"
#define ghos "\u15E3"
#define cher "G"

using namespace std;

bool game_over;

struct walk
{
    short int walk_length;
    int x;
    int y;
    short int back;
};

struct target
{
    int x;
    int y;
};
int x_ghost1, y_ghost1, x_ghost2, y_ghost2, x_ghost3, y_ghost3, x_ghost4, y_ghost4;
vector<target> walk_queue;
vector<walk> BFSArray;
string grid[1600];
string tmp_map[1600];
string username;
int GRID_HEIGHT, GRID_WIDTH;
int x_pacman, y_pacman, score;
int life_counter = 3;
int ghost2_counter = 10, ghost3_counter = 25, ghost4_counter = 45;
int x_cherry1, y_cherry1, x_cherry2, y_cherry2, x_cherry3, y_cherry3, x_cherry4, y_cherry4;
void secondMenu();
void ResetGrid()
{
    // Fills the grid with walls ("\u2588" characters).
    int n = sizeof(grid) / sizeof(grid[0]);
    for (int i = 0; i < n; ++i)
    {
        grid[i] = "\u2588";
    }
}
void ResetGrid();
int XYToIndex(int x, int y);
int IsInBounds(int x, int y);
void Visit(int x, int y);
void PrintGrid();
void addToQueue(int x, int y, int walk_length, int back);
void FindPath(int sx, int sy, int x, int y);

enum eDirection
{
    STOP = 0,
    UP,
    DOWN,
    RIGHT,
    LEFT
};
eDirection dir;

void save()
{
    ofstream outFile(username + "gameData.txt");
    outFile << GRID_WIDTH << ' ' << GRID_HEIGHT << ' ' << life_counter << ' ' << x_pacman << ' ' << y_pacman << ' ' << x_ghost1 << ' ' << y_ghost1 << ' ' << x_ghost2 << ' ' << y_ghost2 << ' ' << x_ghost3 << ' ' << y_ghost3 << ' ' << x_ghost4 << ' ' << y_ghost4 << x_cherry1 << ' ' << y_cherry1 << ' ' << x_cherry2 << ' ' << y_cherry2 << ' ' << x_cherry3 << ' ' << x_cherry3 << ' ' << x_cherry4 << ' ' << y_cherry4 << ' ' << score << ' ' << ghost2_counter << ' ' << ghost3_counter << ' ' << ghost4_counter;
    outFile.close();
    ofstream mapFile(username + "gameMap.txt");
    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            string current_char = grid[XYToIndex(x, y)];
            if (grid[XYToIndex(x, y)] == "\u2588")
            {
                grid[XYToIndex(x, y)] == "#";
            }
            if (x_pacman == x && y_pacman == y)
            {
                current_char = "P";
            }
            if (x_cherry1 == x && y_cherry1 == y)
            {
                current_char = cher;
            }
            if (x_cherry2 == x && y_cherry2 == y)
            {
                current_char = cher;
            }
            if (x_cherry3 == x && y_cherry3 == y)
            {
                current_char = cher;
            }
            if (x_cherry4 == x && y_cherry4 == y)
            {
                current_char = cher;
            }
            if (x_ghost1 == x && y_ghost1 == y)
            {
                current_char = "h";
            }
            if (x_ghost2 == x && y_ghost2 == y)
            {
                current_char = "h";
            }
            if (x_ghost3 == x && y_ghost3 == y)
            {
                current_char = "h";
            }
            if (x_ghost4 == x && y_ghost4 == y)
            {
                current_char = "h";
            }
            mapFile << current_char;
            mapFile << (current_char == "\u2588" ? "\u2588" : "s");
        }
        mapFile << endl;
    }
    mapFile.close();
    //    outFile.write(reinterpret_cast<const char*>(&data), sizeof(info));
}
void load()
{
    char searched_name[30];
    cin >> searched_name;
    bool isFound = false;
    ifstream inFile(username + "gameData.txt");
    if (inFile)
    {
        int cnt, counter = 1;
        while (inFile >> cnt)
        {
            switch (counter)
            {
            case 1:
                GRID_WIDTH = cnt;
                break;
            case 2:
                GRID_HEIGHT = cnt;
                break;
            case 3:
                life_counter = cnt;
                break;
            case 4:
                x_pacman = cnt;
                break;
            case 5:
                y_pacman = cnt;
                break;
            case 6:
                x_ghost1 = cnt;
                break;
            case 7:
                y_ghost1 = cnt;
                break;
            case 8:
                x_ghost2 = cnt;
                break;
            case 9:
                y_ghost2 = cnt;
                break;
            case 10:
                x_ghost3 = cnt;
                break;
            case 11:
                y_ghost3 = cnt;
                break;
            case 12:
                x_ghost4 = cnt;
                break;
            case 13:
                y_ghost4 = cnt;
                break;
            case 14:
                x_cherry1 = cnt;
                break;
            case 15:
                y_cherry1 = cnt;
                break;
            case 16:
                x_cherry2 = cnt;
                break;
            case 17:
                y_cherry2 = cnt;
                break;
            case 18:
                x_cherry3 = cnt;
                break;
            case 19:
                y_cherry3 = cnt;
                break;
            case 20:
                x_cherry4 = cnt;
                break;
            case 21:
                y_cherry4 = cnt;
                break;
            case 22:
                score = cnt;
                break;
            case 23:
                ghost2_counter = cnt;
                break;
            case 24:
                ghost3_counter = cnt;
                break;
            case 25:
                ghost4_counter = cnt;
                break;
            }
            counter++;
        }
    }
    else
    {
        cout << endl
             << "FILE COULD NOT BE OPENED" << endl;
        exit(1);
    }
    inFile.close();

    /*ifstream maplLoad(username + "gameData.txt");
    if (maplLoad)
    {
        char ml;
        char a[GRID_HEIGHT * GRID_WIDTH];
        int x = 0, y = 0;
        while (maplLoad>>ml) {\
            a[XYToIndex(x,y)]=ml;
            if (x==GRID_WIDTH)
            {
                y++;
                x = 0;
            }
            x++;
        }
    }*/
}

void movePacman()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'p':
        case 'P':
            secondMenu();
            break;
        case 'w':
        case 'W':
            dir = UP;
            break;
        case 's':
        case 'S':
            dir = DOWN;
            break;
        case 'a':
        case 'A':
            dir = LEFT;
            break;
        case 'd':
        case 'D':
            dir = RIGHT;
            break;
        }
    }
    switch (dir)
    {
    case UP:
        if (grid[XYToIndex(x_pacman, y_pacman - 1)] != "\u2588")
            y_pacman--;
        break;
    case DOWN:
        if (grid[XYToIndex(x_pacman, y_pacman + 1)] != "\u2588")
            y_pacman++;
        break;
    case RIGHT:
        if (grid[XYToIndex(x_pacman + 1, y_pacman)] != "\u2588")
            x_pacman++;
        break;
    case LEFT:
        if (grid[XYToIndex(x_pacman - 1, y_pacman)] != "\u2588")
            x_pacman--;
        break;
    }
}
void generate_map1()
{
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 15);
    cout << "PLS CHOOSE ONE COLOR FOR GAME:" << endl;
    SetConsoleTextAttribute(hConsole, 8);
    cout << "1-GRAY" << endl;
    SetConsoleTextAttribute(hConsole, 9);
    cout << "2-BLUE" << endl;
    SetConsoleTextAttribute(hConsole, 10);
    cout << "3-GREEN" << endl;
    SetConsoleTextAttribute(hConsole, 11);
    cout << "4-CYAN" << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "5-RED" << endl;
    SetConsoleTextAttribute(hConsole, 13);
    cout << "6-PINK" << endl;
    SetConsoleTextAttribute(hConsole, 14);
    cout << "7-YELLOW" << endl;
    int num3;
    cin >> num3;
    SetConsoleTextAttribute(hConsole, (num3 % 7) + 7);
    system("cls");
    cout << "PLS ENTER WIDTH BETWEEN 20 TO 40" << endl;
    cin >> GRID_WIDTH;
    system("cls");
    cout << "PLS ENTER HEIGHT BETWEEN 20 TO 40" << endl;
    cin >> GRID_HEIGHT;
    x_pacman = GRID_WIDTH / 2;
    y_pacman = (3 * GRID_HEIGHT) / 4;
    x_ghost1 = GRID_WIDTH / 2 - 1;
    y_ghost1 = GRID_HEIGHT / 4 - 1;
    x_ghost2 = GRID_WIDTH / 2 + 1;
    y_ghost2 = GRID_HEIGHT / 4 - 1;
    x_ghost3 = GRID_WIDTH / 2 + 1;
    y_ghost3 = GRID_HEIGHT / 4;
    x_ghost4 = GRID_WIDTH / 2 - 1;
    y_ghost4 = GRID_HEIGHT / 4;
    x_cherry1 = (rand() % GRID_WIDTH);
    y_cherry1 = (rand() % GRID_HEIGHT);
    x_cherry2 = (rand() % GRID_WIDTH);
    y_cherry2 = (rand() % GRID_HEIGHT);
    x_cherry3 = (rand() % GRID_WIDTH);
    y_cherry3 = (rand() % GRID_HEIGHT);
    x_cherry4 = (rand() % GRID_WIDTH);
    y_cherry4 = (rand() % GRID_HEIGHT);
    grid[XYToIndex(x_ghost1, y_ghost1)] = ".";
    grid[XYToIndex(x_ghost2, y_ghost2)] = ".";
    grid[XYToIndex(x_ghost3, y_ghost3)] = ".";
    grid[XYToIndex(x_ghost4, y_ghost4)] = ".";
    // Starting point and top-level control.
    srand(time(0)); // seed random number generator.
    ResetGrid();
    Visit(x_pacman, y_pacman);
    int x_house = GRID_WIDTH / 2;
    int y_house = GRID_HEIGHT / 4;
    grid[XYToIndex(x_pacman + 1, y_pacman)] = ".";
    grid[XYToIndex(x_pacman - 1, y_pacman)] = ".";
    grid[XYToIndex(x_pacman, y_pacman + 1)] = ".";
    grid[XYToIndex(x_pacman, y_pacman - 1)] = ".";
    for (int i = x_house - 2; i <= x_house + 2; i++)
    {
        grid[XYToIndex(i, y_house - 2)] = "\u2588";
        grid[XYToIndex(i, y_house + 2)] = "\u2588";
        grid[XYToIndex(i, y_house - 3)] = ".";
        grid[XYToIndex(i, y_house + 3)] = ".";
        for (int j = y_house - 1; j <= y_house + 1; j++)
        {
            grid[XYToIndex(i, j)] = " ";
        }
    }
    for (int j = y_house - 2; j <= y_house + 2; j++)
    {
        grid[XYToIndex(x_house + 3, j)] = "\u2588";
        grid[XYToIndex(x_house - 3, j)] = "\u2588";
        grid[XYToIndex(x_house + 4, j)] = ".";
        grid[XYToIndex(x_house - 4, j)] = ".";
    }
    grid[XYToIndex(x_house, y_house - 2)] = " ";
    SetConsoleTextAttribute(hConsole, (num3 % 7) + 7);
    PrintGrid();
}
void generate_map2()
{
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 15);
    cout << "PLS CHOOSE ONE COLOR FOR GAME:" << endl;
    SetConsoleTextAttribute(hConsole, 8);
    cout << "1-GRAY" << endl;
    SetConsoleTextAttribute(hConsole, 9);
    cout << "2-BLUE" << endl;
    SetConsoleTextAttribute(hConsole, 10);
    cout << "3-GREEN" << endl;
    SetConsoleTextAttribute(hConsole, 11);
    cout << "4-CYAN" << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "5-RED" << endl;
    SetConsoleTextAttribute(hConsole, 13);
    cout << "6-PINK" << endl;
    SetConsoleTextAttribute(hConsole, 14);
    cout << "7-YELLOW" << endl;
    int num3;
    cin >> num3;
    SetConsoleTextAttribute(hConsole, (num3 % 7) + 7);
    system("cls");
    cout << "PLS ENTER WIDTH BETWEEN 20 TO 40" << endl;
    cin >> GRID_WIDTH;
    system("cls");
    cout << "PLS ENTER HEIGHT BETWEEN 20 TO 40" << endl;
    cin >> GRID_HEIGHT;
    x_pacman = GRID_WIDTH / 2;
    y_pacman = (3 * GRID_HEIGHT) / 4;
    x_ghost1 = GRID_WIDTH / 2 - 1;
    y_ghost1 = GRID_HEIGHT / 4 - 1;
    x_ghost2 = GRID_WIDTH / 2 + 1;
    y_ghost2 = GRID_HEIGHT / 4 - 1;
    x_ghost3 = GRID_WIDTH / 2 + 1;
    y_ghost3 = GRID_HEIGHT / 4;
    x_ghost4 = GRID_WIDTH / 2 - 1;
    y_ghost4 = GRID_HEIGHT / 4;
    x_cherry1 = (rand() % GRID_WIDTH);
    y_cherry1 = (rand() % GRID_HEIGHT);
    x_cherry2 = (rand() % GRID_WIDTH);
    y_cherry2 = (rand() % GRID_HEIGHT);
    x_cherry3 = (rand() % GRID_WIDTH);
    y_cherry3 = (rand() % GRID_HEIGHT);
    x_cherry4 = (rand() % GRID_WIDTH);
    y_cherry4 = (rand() % GRID_HEIGHT);
    grid[XYToIndex(x_ghost1, y_ghost1)] = ".";
    grid[XYToIndex(x_ghost2, y_ghost2)] = ".";
    grid[XYToIndex(x_ghost3, y_ghost3)] = ".";
    grid[XYToIndex(x_ghost4, y_ghost4)] = ".";
    // Starting point and top-level control.
    srand(time(0)); // seed random number generator.
    ResetGrid();
    Visit(x_pacman, y_pacman);
    int x_house = GRID_WIDTH / 2;
    int y_house = GRID_HEIGHT / 4;
    grid[XYToIndex(x_pacman + 1, y_pacman)] = ".";
    grid[XYToIndex(x_pacman - 1, y_pacman)] = ".";
    grid[XYToIndex(x_pacman, y_pacman + 1)] = ".";
    grid[XYToIndex(x_pacman, y_pacman - 1)] = ".";
    for (int i = x_house - 2; i <= x_house + 2; i++)
    {
        grid[XYToIndex(i, y_house - 2)] = "\u2588";
        grid[XYToIndex(i, y_house + 2)] = "\u2588";
        grid[XYToIndex(i, y_house - 3)] = ".";
        grid[XYToIndex(i, y_house + 3)] = ".";
        for (int j = y_house - 1; j <= y_house + 1; j++)
        {
            grid[XYToIndex(i, j)] = " ";
        }
    }
    for (int j = y_house - 2; j <= y_house + 2; j++)
    {
        grid[XYToIndex(x_house + 3, j)] = "\u2588";
        grid[XYToIndex(x_house - 3, j)] = "\u2588";
        grid[XYToIndex(x_house + 4, j)] = ".";
        grid[XYToIndex(x_house - 4, j)] = ".";
    }
    grid[XYToIndex(x_house, y_house - 2)] = " ";
    SetConsoleTextAttribute(hConsole, (num3 % 7) + 7);
    PrintGrid();
}
void generate_map3()
{
    system("cls");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 15);
    cout << "PLS CHOOSE ONE COLOR FOR GAME:" << endl;
    SetConsoleTextAttribute(hConsole, 8);
    cout << "1-GRAY" << endl;
    SetConsoleTextAttribute(hConsole, 9);
    cout << "2-BLUE" << endl;
    SetConsoleTextAttribute(hConsole, 10);
    cout << "3-GREEN" << endl;
    SetConsoleTextAttribute(hConsole, 11);
    cout << "4-CYAN" << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "5-RED" << endl;
    SetConsoleTextAttribute(hConsole, 13);
    cout << "6-PINK" << endl;
    SetConsoleTextAttribute(hConsole, 14);
    cout << "7-YELLOW" << endl;
    int num3;
    cin >> num3;
    SetConsoleTextAttribute(hConsole, 15);
    system("cls");
    cout << "PLS ENTER WIDTH BETWEEN 20 TO 40" << endl;
    cin >> GRID_WIDTH;
    system("cls");
    cout << "PLS ENTER HEIGHT BETWEEN 20 TO 40" << endl;
    cin >> GRID_HEIGHT;
    x_pacman = GRID_WIDTH / 2;
    y_pacman = (3 * GRID_HEIGHT) / 4;
    x_ghost1 = GRID_WIDTH / 2 - 1;
    y_ghost1 = GRID_HEIGHT / 4 - 1;
    x_ghost2 = GRID_WIDTH / 2 + 1;
    y_ghost2 = GRID_HEIGHT / 4 - 1;
    x_ghost3 = GRID_WIDTH / 2 + 1;
    y_ghost3 = GRID_HEIGHT / 4;
    x_ghost4 = GRID_WIDTH / 2 - 1;
    y_ghost4 = GRID_HEIGHT / 4;
    x_cherry1 = (rand() % GRID_WIDTH);
    y_cherry1 = (rand() % GRID_HEIGHT);
    x_cherry2 = (rand() % GRID_WIDTH);
    y_cherry2 = (rand() % GRID_HEIGHT);
    x_cherry3 = (rand() % GRID_WIDTH);
    y_cherry3 = (rand() % GRID_HEIGHT);
    x_cherry4 = (rand() % GRID_WIDTH);
    y_cherry4 = (rand() % GRID_HEIGHT);
    grid[XYToIndex(x_cherry1, y_cherry1)] = ".";
    grid[XYToIndex(x_cherry2, y_cherry2)] = ".";
    grid[XYToIndex(x_cherry3, y_cherry3)] = ".";
    grid[XYToIndex(x_cherry4, y_cherry4)] = ".";
    grid[XYToIndex(x_ghost1, y_ghost1)] = ".";
    grid[XYToIndex(x_ghost2, y_ghost2)] = ".";
    grid[XYToIndex(x_ghost3, y_ghost3)] = ".";
    grid[XYToIndex(x_ghost4, y_ghost4)] = ".";
    // Starting point and top-level control.
    srand(time(0)); // seed random number generator.
    ResetGrid();
    Visit(x_pacman, y_pacman);
    int x_house = GRID_WIDTH / 2;
    int y_house = GRID_HEIGHT / 4;
    grid[XYToIndex(x_pacman + 1, y_pacman)] = ".";
    grid[XYToIndex(x_pacman - 1, y_pacman)] = ".";
    grid[XYToIndex(x_pacman, y_pacman + 1)] = ".";
    grid[XYToIndex(x_pacman, y_pacman - 1)] = ".";
    for (int i = x_house - 2; i <= x_house + 2; i++)
    {
        grid[XYToIndex(i, y_house - 2)] = "\u2588";
        grid[XYToIndex(i, y_house + 2)] = "\u2588";
        grid[XYToIndex(i, y_house - 3)] = ".";
        grid[XYToIndex(i, y_house + 3)] = ".";
        for (int j = y_house - 1; j <= y_house + 1; j++)
        {
            grid[XYToIndex(i, j)] = " ";
        }
    }
    for (int j = y_house - 2; j <= y_house + 2; j++)
    {
        grid[XYToIndex(x_house + 3, j)] = "\u2588";
        grid[XYToIndex(x_house - 3, j)] = "\u2588";
        grid[XYToIndex(x_house + 4, j)] = ".";
        grid[XYToIndex(x_house - 4, j)] = ".";
    }
    grid[XYToIndex(x_house, y_house - 2)] = " ";
    SetConsoleTextAttribute(hConsole, (num3 % 7) + 7);
    PrintGrid();
}
void menuLevelGame()
{
    system("cls");
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 3);
    cout << "╔════════╗" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "║ 1-EASY ║" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "╚════════╝" << endl;
    for (int i = 0; i < 64; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 5);
    cout << "╔══════════╗" << endl;
    for (int i = 0; i < 64; i++)
    {
        cout << " ";
    }
    cout << "║ 2-NORMAL ║" << endl;
    for (int i = 0; i < 64; i++)
    {
        cout << " ";
    }
    cout << "╚══════════╝" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 4);
    cout << "╔════════╗" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "║ 3-HARD ║" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "╚════════╝" << endl;
    int num1;
    score = 0;
    cin >> num1;
    switch (num1)
    {
    case 1:
        generate_map1();
        break;
    case 2:
        generate_map2();
        break;
    case 3:
        generate_map3();
        break;
    }
    while (!game_over)
    {
        movePacman();
        FindPath(x_ghost1, y_ghost1, x_pacman, y_pacman);
        if (walk_queue.size() > 0)
        {
            target next_pos1 = walk_queue[walk_queue.size() - 1];
            if (
                !(next_pos1.x == x_ghost1 && next_pos1.y == y_ghost1 ||
                  next_pos1.x == x_ghost2 && next_pos1.y == y_ghost2 ||
                  next_pos1.x == x_ghost3 && next_pos1.y == y_ghost3 ||
                  next_pos1.x == x_ghost4 && next_pos1.y == y_ghost4))
            {
                // age gilas bod for baraye 4 jahat check kardan  4 jahat
                x_ghost1 = next_pos1.x;
                y_ghost1 = next_pos1.y;
            }
        }
        if (x_ghost1 == x_pacman && y_ghost1 == y_pacman)
        {
            game_over = true;
        }
        if (ghost2_counter == 0)
        {
            FindPath(x_ghost2, y_ghost2, x_pacman, y_pacman);
            if (walk_queue.size() > 0)
            {
                target next_pos1 = walk_queue[walk_queue.size() - 1];
                if (
                    !(next_pos1.x == x_ghost1 && next_pos1.y == y_ghost1 ||
                      next_pos1.x == x_ghost2 && next_pos1.y == y_ghost2 ||
                      next_pos1.x == x_ghost3 && next_pos1.y == y_ghost3 ||
                      next_pos1.x == x_ghost4 && next_pos1.y == y_ghost4))
                {
                    x_ghost2 = next_pos1.x;
                    y_ghost2 = next_pos1.y;
                }
            }
            if (x_ghost2 == x_pacman && y_ghost2 == y_pacman)
            {
                game_over = true;
            }
        }
        if (ghost3_counter == 0)
        {
            FindPath(x_ghost3, y_ghost3, x_pacman, y_pacman);
            if (walk_queue.size() > 0)
            {
                target next_pos1 = walk_queue[walk_queue.size() - 1];
                if (
                    !(next_pos1.x == x_ghost1 && next_pos1.y == y_ghost1 ||
                      next_pos1.x == x_ghost2 && next_pos1.y == y_ghost2 ||
                      next_pos1.x == x_ghost3 && next_pos1.y == y_ghost3 ||
                      next_pos1.x == x_ghost4 && next_pos1.y == y_ghost4))
                {
                    x_ghost3 = next_pos1.x;
                    y_ghost3 = next_pos1.y;
                }
            }
            if (x_ghost3 == x_pacman && y_ghost3 == y_pacman)
            {
                game_over = true;
            }
        }
        if (ghost4_counter == 0)
        {
            FindPath(x_ghost4, y_ghost4, x_pacman, y_pacman);
            if (walk_queue.size() > 0)
            {
                target next_pos1 = walk_queue[walk_queue.size() - 1];
                if (
                    !(next_pos1.x == x_ghost1 && next_pos1.y == y_ghost1 ||
                      next_pos1.x == x_ghost2 && next_pos1.y == y_ghost2 ||
                      next_pos1.x == x_ghost3 && next_pos1.y == y_ghost3 ||
                      next_pos1.x == x_ghost4 && next_pos1.y == y_ghost4))
                {
                    x_ghost4 = next_pos1.x;
                    y_ghost4 = next_pos1.y;
                }
            }
            if (x_ghost4 == x_pacman && y_ghost4 == y_pacman)
            {
                game_over = true;
            }
        }
        if (grid[XYToIndex(x_pacman, y_pacman)] == ".")
        {
            score++;
            grid[XYToIndex(x_pacman, y_pacman)] = " ";
        }
        system("cls");
        PrintGrid();
        Sleep(200);
        if (ghost2_counter > 0)
        {
            ghost2_counter--;
        }
        if (ghost3_counter > 0)
        {
            ghost3_counter--;
        }
        if (ghost4_counter > 0)
        {
            ghost4_counter--;
        }
        if (game_over)
        {
            life_counter--;
            ghost2_counter = 10;
            ghost3_counter = 25;
            ghost4_counter = 45;
            x_pacman = GRID_WIDTH / 2;
            y_pacman = (3 * GRID_HEIGHT) / 4;
            x_ghost1 = GRID_WIDTH / 2;
            y_ghost1 = GRID_HEIGHT / 4 - 1;
            x_ghost2 = GRID_WIDTH / 2;
            y_ghost2 = GRID_HEIGHT / 4 + 1;
            x_ghost3 = GRID_WIDTH / 2 + 1;
            y_ghost3 = GRID_HEIGHT / 4;
            x_ghost4 = GRID_WIDTH / 2 - 1;
            y_ghost4 = GRID_HEIGHT / 4;
            if (life_counter != 0)
            {
                game_over = false;
            }
        }
    }
    system("cls");
    SetConsoleTextAttribute(hConsole, 12);
    cout << "╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  ╔══════════════                                                                                                                                                                          ║" << endl;
    cout << "║  ║                          ╱╲                      ╱╲                    ╱╲           ╔═══════════════              ╔═════════════╗  ╲                    ╱ ╔═══════════════  ╔═══════╗  ║" << endl;
    cout << "║  ║                         ╱  ╲                    ╱  ╲                  ╱  ╲          ║                             ║             ║   ╲                  ╱  ║                 ║       ║  ║" << endl;
    cout << "║  ║                        ╱    ╲                  ╱    ╲                ╱    ╲         ║                             ║             ║    ╲                ╱   ║                 ║       ║  ║" << endl;
    cout << "║  ║                       ╱      ╲                ╱      ╲              ╱      ╲        ║                             ║             ║     ╲              ╱    ║                 ║       ║  ║" << endl;
    cout << "║  ║                      ╱        ╲              ╱        ╲            ╱        ╲       ║                             ║             ║      ╲            ╱     ║                 ║       ║  ║" << endl;
    cout << "║  ║      ═══════╗       ╱━━━━━━━━━━╲            ╱          ╲          ╱          ╲      ╠══════════════               ║             ║       ╲          ╱      ╠══════════════   ║═══════╝  ║" << endl;
    cout << "║  ║             ║      ╱            ╲          ╱            ╲        ╱            ╲     ║                             ║             ║        ╲        ╱       ║                 ║╲         ║" << endl;
    cout << "║  ║             ║     ╱              ╲        ╱              ╲      ╱              ╲    ║                             ║             ║         ╲      ╱        ║                 ║ ╲        ║" << endl;
    cout << "║  ║             ║    ╱                ╲      ╱                ╲    ╱                ╲   ║                             ║             ║          ╲    ╱         ║                 ║  ╲       ║" << endl;
    cout << "║  ║             ║   ╱                  ╲    ╱                  ╲  ╱                  ╲  ║                             ║             ║           ╲  ╱          ║                 ║   ╲      ║" << endl;
    cout << "║  ╚═════════════╝  ╱                    ╲  ╱                    ╲╱                    ╲ ╚═══════════════              ╚═════════════╝            ╲╱           ╚═══════════════  ║    ╲     ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝";
    // int x_start,y_start,x_end,y_end;
    // cin >> x_start >> y_start >> x_end >> y_end;
    // FindPath(x_start,y_start,x_end,y_end);
    // for (int i = 0; i < walk_queue.size(); ++i){
    //     cout << walk_queue[i].x << " " << walk_queue[i].y << endl;
}

void firstMenu()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 5);
    cout << "╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                                                                                                                                                                   ║" << endl;
    cout << "║     ╔════════════╗                                       ╔═════════════════════       ╔═══════════════════╦═══════════════════╗                   ╱╲                  ╔══════════════════╗        ║" << endl;
    cout << "║     ║            ║                  ╱╲                   ║                            ║                   ║                   ║                  ╱  ╲                 ║                  ║        ║" << endl;
    cout << "║     ║            ║                 ╱  ╲                  ║                            ║                   ║                   ║                 ╱    ╲                ║                  ║        ║" << endl;
    cout << "║     ║            ║                ╱    ╲                 ║                            ║                   ║                   ║                ╱      ╲               ║                  ║        ║" << endl;
    cout << "║     ║            ║               ╱      ╲                ║                            ║                   ║                   ║               ╱        ╲              ║                  ║        ║" << endl;
    cout << "║     ║            ║              ╱        ╲               ║                            ║                   ║                   ║              ╱          ╲             ║                  ║        ║" << endl;
    cout << "║     ║            ║             ╱          ╲              ║                            ║                   ║                   ║             ╱            ╲            ║                  ║        ║" << endl;
    cout << "║     ║════════════╝            ╱━━━━━━━━━━━━╲             ║                            ║                   ║                   ║            ╱━━━━━━━━━━━━━━╲           ║                  ║        ║" << endl;
    cout << "║     ║                        ╱              ╲            ║                            ║                   ║                   ║           ╱                ╲          ║                  ║        ║" << endl;
    cout << "║     ║                       ╱                ╲           ║                            ║                   ║                   ║          ╱                  ╲         ║                  ║        ║" << endl;
    cout << "║     ║                      ╱                  ╲          ║                            ║                   ║                   ║         ╱                    ╲        ║                  ║        ║" << endl;
    cout << "║     ║                     ╱                    ╲         ║                            ║                   ║                   ║        ╱                      ╲       ║                  ║        ║" << endl;
    cout << "║     ║                    ╱                      ╲        ║                            ║                   ║                   ║       ╱                        ╲      ║                  ║        ║" << endl;
    cout << "║     ║                   ╱                        ╲       ║                            ║                   ║                   ║      ╱                          ╲     ║                  ║        ║" << endl;
    cout << "║     ║                  ╱                          ╲      ╚═════════════════════       ║                   ║                   ║     ╱                            ╲    ║                  ║        ║" << endl;
    cout << "║     ║                                                                                                                                                                                             ║" << endl;
    cout << "╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝";
    Sleep(5000);
    system("cls");
    SetConsoleTextAttribute(hConsole, 3);
    cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                                                                                                                                                  ║" << endl;
    cout << "║                                                                                                                                                                                  ║" << endl;
    cout << "║                                                                                                                                                                                  ║" << endl;
    cout << "║     ║                      ║    ╔══════════════════════╗     ╔══════════════════════╗    ║                      ║     ╔══════════════════════╗     ╔══════════════════════╗      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ╠══════════════════════╣    ║                      ║     ║                      ║    ╠══════════════════════╣     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ║                      ║     ║                      ║    ║                      ║     ║                      ║     ║                      ║      ║" << endl;
    cout << "║     ║                      ║    ╚══════════════════════╝     ╚══════════════════════╝    ║                      ║     ╚══════════════════════╝     ╚══════════════════════╝      ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝";
    Sleep(5000);
    system("cls");
    SetConsoleTextAttribute(hConsole, 15);
    cout << "PLS ENTER YOUR NAME: ";
    cin >> username;
    system("cls");
    for (int i = 0; i < 58; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 10);
    string start = "WELLCOME TO HOOHOO GAME";
    Sleep(600);
    for (auto ch : start)
    {
        cout << ch;
        Sleep(40);
    }
    cout << endl
         << endl
         << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 5);
    cout << "╔══════╗" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "║ MENU ║" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "╚══════╝" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 3);
    cout << "╔════════════╗" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "║ 1-NEW GAME ║" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "╚════════════╝" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 6);
    cout << "╔═════════════╗" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "║ 2-LOAD GAME ║" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "╚═════════════╝" << endl;
    for (int i = 0; i < 63; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 13);
    cout << "╔═══════════╗" << endl;
    for (int i = 0; i < 63; i++)
    {
        cout << " ";
    }
    cout << "║ 3-RANKING ║" << endl;
    for (int i = 0; i < 63; i++)
    {
        cout << " ";
    }
    cout << "╚═══════════╝" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 4);
    cout << "╔════════╗" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "║ 4-EXIT ║" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "╚════════╝" << endl;
    int num2;
    cin >> num2;
    switch (num2)
    {
    case 1:
        menuLevelGame();
        break;
    case 2:
        // load();
        break;
    case 3:
        // ranking
        break;
    case 4:
        return;
    }
}
void secondMenu()
{
    system("cls");
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
    cout << "╔═══════╗" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "║ PAUSE ║" << endl;
    for (int i = 0; i < 65; i++)
    {
        cout << " ";
    }
    cout << "╚═══════╝" << endl;
    SetConsoleTextAttribute(hConsole, 10);
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "╔═════════════╗" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "║  1-RESUME   ║" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "╚═════════════╝" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 3);
    cout << "╔═════════════╗" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "║ 2-SAVE GAME ║" << endl;
    for (int i = 0; i < 62; i++)
    {
        cout << " ";
    }
    cout << "╚═════════════╝" << endl;
    for (int i = 0; i < 64; i++)
    {
        cout << " ";
    }
    SetConsoleTextAttribute(hConsole, 4);
    cout << "╔════════╗" << endl;
    for (int i = 0; i < 64; i++)
    {
        cout << " ";
    }
    cout << "║ 3-EXIT ║" << endl;
    for (int i = 0; i < 64; i++)
    {
        cout << " ";
    }
    cout << "╚════════╝" << endl;
    int num4;
    cin >> num4;
    // info s;
    /*s.arr[1600] = grid[1600];
    strcpy_s(s.username, username);
    s.num4 = num4;
    s.GRID_WIDTH = GRID_WIDTH;
    s.GRID_HEIGHT = GRID_HEIGHT;
    s.life_counter = life_counter;
    s.x_ghost1 = x_ghost1;
    s.y_ghost1 = y_ghost1;
    s.x_ghost2 = x_ghost2;
    s.y_ghost2 = y_ghost2;
    s.x_ghost3 = x_ghost3;
    s.y_ghost3 = y_ghost3;
    s.x_ghost4 = x_ghost4;
    s.y_ghost4 = y_ghost4;
    s.x_pacman = x_pacman;
    s.y_pacman = y_pacman;
    s.score = score;
    s.ghost2_counter = ghost2_counter;
    s.ghost3_counter = ghost3_counter;
    s.ghost4_counter = ghost4_counter*/
    switch (num4)
    {
    case 1:
        break;
    case 2:
        save();
        break;
    case 3:
        // load();
        break;
    }
}
// DFS MAP
int XYToIndex(int x, int y)
{
    // Converts the two-dimensional index pair (x,y) into a
    // single-dimensional index. The result is y * ROW_WIDTH + x.
    return y * GRID_WIDTH + x;
}
int IsInBounds(int x, int y)
{
    // Returns "true" if x and y are both in-bounds.
    if (x < 0 || x >= GRID_WIDTH)
        return false;
    if (y < 0 || y >= GRID_HEIGHT)
        return false;
    return true;
}
// This is the recursive function we will code in the next project
void Visit(int x, int y)
{
    // Starting at the given index, recursively visits every direction in a
    // randomized order.
    // Set my current location to be an empty passage.
    int wall_count = 0;
    for (int dx = -1; dx < 2; dx++)
    {
        for (int dy = -1; dy < 2; dy++)
        {
            if (grid[XYToIndex(x + dx, y + dy)] == "\u2588")
            {
                wall_count++;
            }
        }
    }
    if (wall_count < 6)
    {
        return;
    }

    grid[XYToIndex(x, y)] = '.';
    // Create an local array containing the 4 directions and shuffle their order.
    int dirs[4];
    dirs[0] = NORTH;
    dirs[1] = EAST;
    dirs[2] = SOUTH;
    dirs[3] = WEST;
    for (int i = 0; i < 4; ++i)
    {
        int r = rand() & 3;
        int temp = dirs[r];
        dirs[r] = dirs[i];
        dirs[i] = temp;
    }
    // Loop through every direction and attempt to Visit that direction.
    for (int i = 0; i < 4; ++i)
    {
        // dx,dy are offsets from current location. Set them based
        // on the next direction I wish to try.
        int dx = 0, dy = 0;
        switch (dirs[i])
        {
        case NORTH:
            dy = -1;
            break;
        case SOUTH:
            dy = 1;
            break;
        case EAST:
            dx = 1;
            break;
        case WEST:
            dx = -1;
            break;
        }
        // Find the (x,y) coordinates of the grid cell 2 spots
        // away in the given direction.
        int x2 = x + 2 * dx;
        int y2 = y + 2 * dy;
        if (IsInBounds(x2, y2))
        {
            if ((grid[XYToIndex(x2, y2)] == "\u2588") && (grid[XYToIndex(x2 - dx, y2 - dy)] != "."))
            {
                // (x2,y2) has not been visited yet... knock down the
                // wall between my current position and that position
                // Recursively Visit (x2,y2)
                Visit(x2 - dx, y2 - dy);
            }
        }
    }
}
void PrintGrid()
{
    // Displays the finished maze to the screen.
    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            string current_char = grid[XYToIndex(x, y)];
            if (x_pacman == x && y_pacman == y)
            {
                current_char = "\u15E7";
            }
            if (x_cherry1 == x && y_cherry1 == y)
            {
                current_char = "G";
            }
            if (x_cherry2 == x && y_cherry2 == y)
            {
                current_char = "G";
            }
            if (x_cherry3 == x && y_cherry3 == y)
            {
                current_char = "G";
            }
            if (x_cherry4 == x && y_cherry4 == y)
            {
                current_char = "G";
            }
            if (x_ghost1 == x && y_ghost1 == y)
            {
                current_char = "\u15E3";
            }
            if (x_ghost2 == x && y_ghost2 == y)
            {
                current_char = "\u15E3";
            }
            if (x_ghost3 == x && y_ghost3 == y)
            {
                current_char = "\u15E3";
            }
            if (x_ghost4 == x && y_ghost4 == y)
            {
                current_char = "\u15E3";
            }
            cout << current_char;
            cout << (current_char == "\u2588" ? "\u2588" : " ");
        }
        cout << endl;
    }
    cout << "SCORE: " << score << endl;
    for (int i = 0; i < life_counter; i++)
    {
        cout << "\u2665 ";
    }
}
// BFS GHOSTS
void addToQueue(int x, int y, int walk_length, int back)
{
    int i = XYToIndex(x, y);
    if (tmp_map[i] != "\u2588")
    {
        int j = XYToIndex(x, y);
        tmp_map[j] = "\u2588";
        walk tmp;
        tmp.x = x;
        tmp.y = y;
        tmp.walk_length = walk_length;
        tmp.back = back;
        BFSArray.push_back(tmp);
    }
}
void FindPath(int sx, int sy, int x, int y)
{
    memcpy(tmp_map, grid, sizeof(grid));
    BFSArray.clear();
    walk tmp;
    tmp.x = sx;
    tmp.y = sy;
    tmp.walk_length = 0;
    tmp.back = -1;
    BFSArray.push_back(tmp);

    int i = 0;
    while (i < BFSArray.size())
    {
        if (BFSArray[i].x == x && BFSArray[i].y == y)
        {
            walk_queue.clear();
            target tmp2;
            while (BFSArray[i].walk_length != 0)
            {
                tmp2.x = BFSArray[i].x;
                tmp2.y = BFSArray[i].y;
                walk_queue.push_back(tmp2);

                i = BFSArray[i].back;
            }

            break;
        }
        addToQueue(BFSArray[i].x + 1, BFSArray[i].y, BFSArray[i].walk_length + 1, i);
        addToQueue(BFSArray[i].x - 1, BFSArray[i].y, BFSArray[i].walk_length + 1, i);
        addToQueue(BFSArray[i].x, BFSArray[i].y + 1, BFSArray[i].walk_length + 1, i);
        addToQueue(BFSArray[i].x, BFSArray[i].y - 1, BFSArray[i].walk_length + 1, i);
        i++;
    }

    BFSArray.clear();
}
int main()
{
    firstMenu();
    return 0;
}