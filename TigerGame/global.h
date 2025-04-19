//
// Created by NTMat on 4/19/2025.
//

#ifndef TIGERGAME_GLOBAL_H
#define TIGERGAME_GLOBAL_H


enum Color_t { RED, BLUE };

struct Point_t {
    int row, col;
};

struct Token_t {
    Color_t color;
    Point_t location;
};

#endif //TIGERGAME_GLOBAL_H
