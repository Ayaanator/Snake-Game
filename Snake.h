#pragma once
#include "Position.h"

struct Snake
{
    Position position{ 5, 10 };
    Position direction{ 1, 0 };

    int size{ 1 };

    std::vector<char*> body;
    std::vector<char*> all_positions;
};