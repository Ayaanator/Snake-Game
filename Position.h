#pragma once

struct Position
{
    int y;
    int x;

    Position operator+(Position lhs)
    {
        return { y + lhs.y, x + lhs.x };
    }

    bool operator==(Position lhs)
    {
        return y == lhs.y && x == lhs.x;
    }

    void operator=(Position lhs)
    {
        y = lhs.y;
        x = lhs.x;
    }

    void operator+=(Position lhs)
    {
        *this = *this + lhs;
    }
};