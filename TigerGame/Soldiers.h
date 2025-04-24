//
// Created by NTMat on 4/19/2025.
//

#ifndef TIGERGAME_SOLDIERS_H
#define TIGERGAME_SOLDIERS_H

#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "BearGame.h"
#include "constants.h"

using namespace std;

// game phases for soldiers
enum Phase {START, MID, END};

// diagonal edges
vector<pair<Point_t,Point_t>> diags = {
    // lair
    make_pair(Point_t(0,4), Point_t(1,3)),
    make_pair(Point_t(0,4), Point_t(1,5)),
    make_pair(Point_t(1,3), Point_t(2,2)),
    make_pair(Point_t(1,3), Point_t(2,4)),
    make_pair(Point_t(1,5), Point_t(2,4)),
    make_pair(Point_t(1,5), Point_t(2,6)),
    make_pair(Point_t(2,2), Point_t(3,3)),
    make_pair(Point_t(2,4), Point_t(3,3)),
    make_pair(Point_t(2,4), Point_t(3,5)),
    make_pair(Point_t(2,6), Point_t(3,5)),
    make_pair(Point_t(3,3), Point_t(4,4)),
    make_pair(Point_t(3,5), Point_t(4,4)),

    // rest of board
    make_pair(Point_t(4,4), Point_t(5,3)),
    make_pair(Point_t(4,4), Point_t(5,5)),
    make_pair(Point_t(5,3), Point_t(6,2)),
    make_pair(Point_t(5,5), Point_t(6,6)),
    make_pair(Point_t(6,2), Point_t(7,1)),
    make_pair(Point_t(6,6), Point_t(7,7)),
    make_pair(Point_t(7,1), Point_t(8,0)),
    make_pair(Point_t(7,7), Point_t(8,8)),
    make_pair(Point_t(8,0), Point_t(9,1)),
    make_pair(Point_t(8,8), Point_t(9,7)),
    make_pair(Point_t(9,1), Point_t(10,2)),
    make_pair(Point_t(9,7), Point_t(10,6)),
    make_pair(Point_t(10,2), Point_t(11,3)),
    make_pair(Point_t(10,6), Point_t(11,5)),
    make_pair(Point_t(11,3), Point_t(12,4)),
    make_pair(Point_t(11,5), Point_t(12,4)),
};

//check if location is on grid
//uses validLocationGrid from Booth's BearGame.h
bool validLocation(Point_t p){
    bool flag = true;
    int row = p.row;
    int col = p.col;

    if(flag) flag = row >= 0;
    if(flag) flag = row < GRID_ROW;
    if(flag) flag = col >= 0;
    if(flag) flag = col < GRID_COL;
    if(flag) flag = validLocationGrid[row][col];
    return 	 flag;
}

//check if move is legal
bool legalMove(vector<Token_t> game, Move_t move) {
    // check if on grid
    if (!validLocation(move.destination)) { return false;}

    // check if diagnoal
    if ((move.token.location.row != move.destination.row && move.token.location.col != move.destination.col) &&
        (find(diags.begin(), diags.end(), make_pair(move.token.location, move.destination)) == diags.end() ||
        find(diags.begin(), diags.end(), make_pair(move.destination, move.token.location)) == diags.end())) {
        return false;
    }

    // check if in lion's den
    if (move.destination.row < 4 && (move.token.location.row == move.destination.row ||
        move.token.location.col == move.destination.col)) {
        return false;
    }

    // check if interfering with other player
    for (auto i = game.begin(); i != game.end(); i++) {
        if (move.destination == i->location) {
            return false;
        }
    }

    return true;
}

// function to determine phase of game
// can refine definition later
// just doing start and middle for now
Phase getPhase(vector<Token_t> game) {
    int count = 0;
    Phase p;

    // defining start as > 3 soldiers still in back row
    for (int i = 1; i < game.size(); i++) {
        if (game[i].location.row == 12) {
            count++;
        }
    }
    if (count > 3) {
        p = START;
    }
    else { p = MID;}

    return p;
}

// geometric distance between two points
double dist(Point_t p1, Point_t p2) {
    return sqrt((p1.row - p2.row) * (p1.row - p2.row) +
            (p1.col - p2.col) * (p1.col - p2.col));
}

// returns point where tiger could jump to over given man
// point holds (-1,-1) if no point exists
Point_t checkJump(vector<Token_t> game, int man) {
    Point_t jmp(-1,-1);

    // first check if tiger is in range
    if (dist(game[0].location, game[man].location) < 2) {
        // check points surrounding man
        if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row, game[man].location.col + 1)))) {
            jmp = Point_t(game[man].location.row, game[man].location.col + 1);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row, game[man].location.col - 1)))) {
            jmp = Point_t(game[man].location.row, game[man].location.col - 1);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row + 1, game[man].location.col)))) {
            jmp = Point_t(game[man].location.row + 1, game[man].location.col);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row - 1, game[man].location.col)))) {
            jmp = Point_t(game[man].location.row - 1, game[man].location.col);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row + 1, game[man].location.col + 1)))) {
            jmp = Point_t(game[man].location.row + 1, game[man].location.col + 1);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row + 1, game[man].location.col - 1)))) {
            jmp = Point_t(game[man].location.row + 1, game[man].location.col - 1);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row - 1, game[man].location.col + 1)))) {
            jmp = Point_t(game[man].location.row - 1, game[man].location.col + 1);
        }
        else if (legalMove(game, Move_t(game[0], Point_t(game[man].location.row - 1, game[man].location.col - 1)))) {
            jmp = Point_t(game[man].location.row - 1, game[man].location.col - 1);
        }
    }

    return jmp;
}

Move_t randomMove_soldiers(vector<Token_t> game) {
    srand(time(0));
    int man = (rand() % (game.size() - 1)) + 1;

    Point_t destination = game[man].location;
    if (rand() % 2) {
        destination.row = game[man].location.row + (rand() % 3 - 1);
    }
    else {
        destination.col = game[man].location.col + (rand() % 3 - 1);
    }

    while (!legalMove(game, Move_t(game[man], destination))) {
        man = (rand() % (game.size() - 1)) + 1;
        destination = game[man].location;
        if (rand() % 2) {
            destination.row = game[man].location.row + (rand() % 3 - 1);
        }
        else {
            destination.col = game[man].location.col + (rand() % 3 - 1);
        }
    }

    return Move_t(game[man], destination);
}

// Soldiers function
Move_t  Move_NoahsNaiveNights_Men(vector<Token_t> game, Color_t turn) {
    int tiger = 0;
    Phase p = getPhase(game);

    switch (p) {
        // want to try to move pieces to center while sticking together
        case START:

        break;

        // control center and force tiger to corners
        case MID:

        break;

        case END:

        break;
    }

    return randomMove_soldiers(game);
}

#endif //TIGERGAME_SOLDIERS_H
