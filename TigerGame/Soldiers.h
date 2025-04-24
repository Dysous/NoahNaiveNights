//
// Created by NTMat on 4/19/2025.
//

#ifndef TIGERGAME_SOLDIERS_H
#define TIGERGAME_SOLDIERS_H

#include <random>

#include "Booth/constants.h"

using namespace std;

// game phases for soldiers
enum Phase {START, MID, END};

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
    if (!validLocation(move.destination)) { return false;}

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
