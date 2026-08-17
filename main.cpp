#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <unistd.h>

void fill_gb(char** gb, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            gb[i][j] = '-';
}

char** init_gb(int rows, int cols)
{
    char** gb = new char*[rows] {};

    for (int i = 0; i < rows; ++i) 
        gb[i] = new char[cols] {};

    return gb;
}

char** init_game(const char* fileName, int& rows, int& cols, int& alive_cells)
{
    std::ifstream fin{ fileName };
    
    if (!fin.is_open())
        return nullptr;
    
    fin >> rows >> cols;

    char** gb = init_gb(rows, cols);
    fill_gb(gb, rows, cols);

    int row, col;
    while (fin >> row >> col) 
    {
        gb[row][col] = '*';
        alive_cells++;
    }

    return gb;
}

void free_gb(char** gb, int rows)
{
    for (int i = 0; i < rows; i++)
        delete[] gb[i];
    delete[] gb;
    gb = nullptr;
}

void print_gb(char** gb, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
            std::cout << gb[i][j] << ' ';
        std::cout << '\n';
    }
}

void print_stats(int alive_cells, int generation) 
{
    std::cout << "Generation: " << generation << ". Alive cells: " << alive_cells << std::endl;;
}

void save_generation(char** src, char** des, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            des[i][j] = src[i][j];
}

bool is_stable_generation(char** gb1, char** gb2, int rows, int cols)
{
    bool ok = true;
    for (int i = 0; (i < rows) && ok; i++)
        for (int j = 0; (j < cols) && ok; j++)
            ok = (gb1[i][j] == gb2[i][j]);
    return ok;
}

int number_alive_cells(char** gb, int row, int col, int left, int right, int up, int bottom)
{
    int res = 0;
    for (int i = up; i <= bottom; i++)
        for (int j = left; j <= right; j++)
            if ((gb[i][j] == '*') && ((i != row) || (j != col)))
                res++;
    return res;
}

void process_next_generation(char** current, char** next, int rows, int cols, int& alive_cells)
{
    alive_cells = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            int up = std::clamp(i - 1, 0, rows - 1);
            int bottom = std::clamp(i + 1, 0, rows - 1);
            int left = std::clamp(j - 1, 0, cols - 1);
            int right = std::clamp(j + 1, 0, cols - 1);
            
            int nAliveCells = number_alive_cells(current, i, j, left, right, up, bottom);
            bool isAlive = current[i][j] == '*';

            if (isAlive && (nAliveCells == 2 || nAliveCells == 3)) 
            {
                next[i][j] = '*';
                ++alive_cells;
            } 
            else if (!isAlive && nAliveCells == 3) 
            {
                next[i][j] = '*';
                ++alive_cells;
            } 
            else 
            {
                next[i][j] = '-';
            }

        }
}

void print_current_generation(char** current, int rows, int cols, int alive_cells, int generation)
{
    sleep(1);
    std::system("cls");
    print_gb(current, rows, cols);
    print_stats(alive_cells, generation);
}

int game_loop(char** gb, int rows, int cols, int alive_cells)
{
    char** save_gb = init_gb(rows, cols);
    bool isGameOver = false;
    int generation = 1;

    while (!isGameOver)
    {
        // Сохраняем текущее поколение
        save_generation(gb, save_gb, rows, cols);

        // Обработка следующего поколения
        generation++;
        process_next_generation(save_gb, gb, rows, cols, alive_cells);

        
        if (alive_cells == 0) 
        {
            print_current_generation(gb, rows, cols, alive_cells, generation);
            std::cout << "All cells are dead. Game over" << std::endl;
            isGameOver = true;
        }
        else if (is_stable_generation(gb, save_gb, rows, cols))
        {
            print_current_generation(gb, rows, cols, alive_cells, generation);
            std::cout << "The world has stagnated. Game over" << std::endl;
            isGameOver = true;
        }
        else // Продолжаем
        {
            // Вывод текущего поколения
            print_current_generation(gb, rows, cols, alive_cells, generation);
        }
    }

    return EXIT_SUCCESS;
}

int main() 
{
    char** gb;
    int rows = 0, cols = 0;
    int alive_cells = 0;

    if (!(gb = init_game("start.ini", rows, cols, alive_cells)))
        return EXIT_FAILURE;

    int ret = game_loop(gb, rows, cols, alive_cells);

    free_gb(gb, rows);

    return ret;
}