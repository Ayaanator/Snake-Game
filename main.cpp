#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <conio.h>
#include <fstream>
#include <thread>

#include <io.h>
#include <fcntl.h>

#include <Windows.h>

#include "Snake.h"
#include "RandomNumber.h"

#define snake L"\x25A1"
#define apple L"\x25CB"
#define square L"\x25A0"

using namespace std::literals::chrono_literals;

bool game_over{false};

void print_field(std::vector<std::vector<char>> &f);

void move(Snake &s, std::vector<std::vector<char>> &f);
void input(Snake &s);
void beep(bool &b, int &f, int &d);
std::vector<int> get_scores(std::string n);

int main()
{
    //int length;
    //int width;

    std::cout << std::fixed << std::setprecision(2);

    //std::cout << "Enter length: "; std::cin >> length;
    //std::cout << "Enter width: "; std::cin >> width; std::cout << std::endl << std::endl;

    // 10 by 21 square field (2D array of chars)
    std::vector<std::vector<char>> field(10, std::vector<char>(21, 'b'));
    // Main player
    Snake player;

    // Places starting apple at random location
    bool apple_placement_successful{false};
    while (!apple_placement_successful)
    {

        int rand_y{rnd::rand_num(0, field.size() - 1)};
        int rand_x{rnd::rand_num(0, field[0].size() - 1)};

        if (rand_y != player.position.y && rand_x != player.position.x)
        {
            field[rand_y][rand_x] = 'a';
            apple_placement_successful = true;
        }
    }

    std::this_thread::sleep_for(1s);

    std::thread move_thread(move, std::ref(player), std::ref(field));
    std::thread input_thread(input, std::ref(player));

    move_thread.join();
    input_thread.join();
}

void print_field(std::vector<std::vector<char>> &f)
{
    for (auto r : f)
    {
        for (auto c : r)
        {
            // Sets printing mode to _O_U16TEXT to print special characters from pre-defined values
            _setmode(_fileno(stdout), _O_U16TEXT);

            if (c == 'a')
            {
                wprintf(apple);
            }
            else if (c == 'b')
            {
                wprintf(square);
            }
            else if (c == 's')
            {
                wprintf(snake);
            }
        }

        // Sets printing mode back to _O_TEXT to prevent any errors from using normal characters
        _setmode(_fileno(stdout), _O_TEXT);
        std::cout << std::endl;
    }
}

void move(Snake &s, std::vector<std::vector<char>> &f)
{
    bool beep_bool{false};
    int frequency{3000};
    int duration{500};
    int del{1};

    bool apple_eaten{false};

    std::thread beep_thread(beep, std::ref(beep_bool), std::ref(frequency), std::ref(duration));

    while (!game_over)
    {
        system("cls");

        std::cout << "Left click to Pause | Right click to Continue\n\n";

        print_field(f);

        std::cout << "\nScore : " << s.size << std::endl;

        if (s.size > s.body.size())
        {
            // Modifies field so it's properly displayed
            f[s.position.y][s.position.x] = 's';
            s.body.push_back(&(f[s.position.y][s.position.x]));
            s.all_positions.push_back(&(f[s.position.y][s.position.x]));
        }
        else
        {
            // Removes tail of snake
            *(s.body[0]) = 'b';
            s.body.erase(s.body.begin());
        }

        // Makes snake head go forward
        s.position += s.direction;

        // If snake is out of bounds or touching itself
        if (s.position.y == f.size() || s.position.y == -1 || s.position.x == f[0].size() || s.position.x == -1 || f[s.position.y][s.position.x] == 's')
        {
            game_over = true;
            std::cout << std::endl
                      << "Game Over" << std::endl;

            std::fstream file{"scores.txt"};

            // Displays statistics
            std::vector<int> scores = get_scores("scores.txt");
            scores.push_back(s.size);

            for (int num : scores)
            {
                file << num << std::endl;
            }

            int high_score{[](std::vector<int> v)
                           {
                               int high{0};

                               for (int num : v)
                               {
                                   if (num > high)
                                   {
                                       high = num;
                                   }
                               }

                               return high;
                           }(scores)};

            double average_score{(static_cast<double>(std::accumulate(scores.begin(), scores.end(), 0)) / static_cast<double>(scores.size()))};

            std::cout << "\nHigh score: " << high_score << std::endl;
            std::cout << "Average score: " << average_score << std::endl;

            std::cout << "All scores: ";

            for (int i = 0; i < scores.size(); i++)
            {
                std::cout << scores[i];

                if (i != scores.size() - 1)
                {
                    std::cout << " | ";
                }
            }

            file.close();
            std::cout << "\n\nPress any key to close application..." << std::endl;
        }
        else
        {
            // If touching apple
            if (f[s.position.y][s.position.x] == 'a')
            {
                s.size++;

                bool apple_placement_successful{false};
                apple_eaten = true;

                // Place apple at random location
                while (!apple_placement_successful)
                {
                    int rand_y{rnd::rand_num(0, f.size() - 1)};
                    int rand_x{rnd::rand_num(0, f[0].size() - 1)};

                    if (f[rand_y][rand_x] != 's')
                    {
                        f[rand_y][rand_x] = 'a';
                        apple_placement_successful = true;
                    }
                }

                // Places a snake part at the back of the tail
                int num{1};
                int index{static_cast<int>(s.all_positions.size() + num - s.size)};
                
                *(s.all_positions[index]) = 's';
                s.body.insert(s.body.begin(), s.all_positions[index]);
            }
            else
            {
                // Modifies field so it's properly displayed
                f[s.position.y][s.position.x] = 's';
                s.body.push_back(&(f[s.position.y][s.position.x]));
                s.all_positions.push_back(&(f[s.position.y][s.position.x]));
            }

            if (s.size < s.body.size())
            {
                // Removes tail of snake
                *(s.body[0]) = 'b';
                s.body.erase(s.body.begin());
            }
        }

        std::this_thread::sleep_for(0.1s);

        if (apple_eaten)
        {
            beep_bool = true;
            apple_eaten = false;
        }
    }

    beep_thread.join();
}

void input(Snake &s)
{
    bool beep_bool{false};
    int frequency{1000};
    int duration{100};
    int del{0};

    std::thread beep_thread(beep, std::ref(beep_bool), std::ref(frequency), std::ref(duration));

    while (!game_over)
    {
        // Gets key input and changes direction depending on which key is pressed
        char c = _getch();
        c = std::toupper(c);

        if (c == 'W' || c == 'A' || c == 'S' || c == 'D')
        {
            if (c == 'D')
            {
                if (s.direction == Position{1, 0} || s.direction == Position{-1, 0})
                {
                    s.direction = {0, 1};
                }
            }
            if (c == 'A')
            {
                if (s.direction == Position{1, 0} || s.direction == Position{-1, 0})
                {
                    s.direction = {0, -1};
                }
            }
            if (c == 'S')
            {
                if (s.direction == Position{0, 1} || s.direction == Position{0, -1})
                {
                    s.direction = {1, 0};
                }
            }
            if (c == 'W')
            {
                if (s.direction == Position{0, 1} || s.direction == Position{0, -1})
                {
                    s.direction = {-1, 0};
                }
            }

            beep_bool = true;
        }
    }

    beep_thread.join();
}

void beep(bool &b, int &f, int &d)
{
    while (!game_over)
    {
        if (b)
        {
            Beep(f, d);
            b = false;
        }
    }
}

std::vector<int> get_scores(std::string n)
{
    std::fstream i_f{n};
    std::vector<int> temp;
    int num{0};

    while (i_f >> num)
    {
        temp.push_back(num);
    }

    return temp;
}