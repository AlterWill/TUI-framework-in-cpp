#pragma once
#include <string>

struct Cell{
    std::string glyph;
};
struct boxOutlineDetails{
    std::string horizontal;
    std::string vertical;
    std::string topLeft;
    std::string topRight;
    std::string bottomLeft;
    std::string bottomRight;
};
struct bounds{
    int x;
    int y;
    int height;
    int width;
};
