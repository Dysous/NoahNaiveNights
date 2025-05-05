//replace int with size_t in for loops using size

//iterate through all possible moves, validate moves then sort into soldiers or open
    // - getMoves() >
    //iterate through soldiers and see if any can be eaten
        // - findSoldiers() >
        // - eatSoldier() >
        //iterate through open and make the move closest to center
        // - findNextMove() <
        // - getDistance() >
#ifndef TigerStrat_h
#define TigerStrat_h

#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "BearGame.h"
#include "constants.h"
#include <cmath>

using namespace std;

// diagonal edges
vector<pair<Point_t, Point_t>> diags = {
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
    return      flag;
}

//check if move is legal
bool legalMove(vector<Token_t> game, Move_t move) {

    // check if on grid
    if (!validLocation(move.destination)) { return false;}

    Point_t source = move.token.location;
    Point_t dest = move.destination;


    int destRow = dest.row - source.row;
    int destCol = dest.col - source.col;

    //check orthogonal first
    bool ok = (abs(destRow) + abs(destCol) == 1);

    // check if diagnoal (added reverse too)
    if (!ok && abs(destRow) == 1 && abs(destCol) == 1) {
        // check either orientation in your diag list
        auto forward  = make_pair(source, dest);
        auto backward = make_pair(dest, source);
        ok = (find(diags.begin(), diags.end(), forward ) != diags.end()) || (find(diags.begin(), diags.end(), backward) != diags.end());
    }
    if (!ok) return false;
    
    // check if interfering with other player
    for (auto i = game.begin(); i != game.end(); i++) {
        if (move.destination == i->location) {
            return false;
        }
    }

    return true;
}


// geometric distance between two points
double dist(Point_t p1, Point_t p2) {
    return sqrt((p1.row - p2.row) * (p1.row - p2.row) +
                (p1.col - p2.col) * (p1.col - p2.col));
}

vector<Point_t> getMoves(vector<Token_t> players) {
    //Get tiger's current location
    Point_t tigerLoc = players[0].location;

    //Iterate through all nearby moves
    vector<Point_t> possMoves;
    
    for (int r = tigerLoc.row - 1; r <= tigerLoc.row + 1; r++) {
        for (int c = tigerLoc.col - 1; c <= tigerLoc.col + 1; c++) {
            if (r != tigerLoc.row && c != tigerLoc.col) {
                Move_t move;
                move.token = players[0];
                
                Point_t dest;
                dest.row = r;
                dest.col = c;
                
                move.destination = dest;
                
                //If move is possible/legal add it to list of possible moves
                if (legalMove(players, move)) {
                    possMoves.push_back(dest);
                }
            }
        }
    }

    return possMoves;
}

Move_t nextMove(vector<Token_t> players, vector<Point_t> possMoves) {
    double distFromCenter = INFINITY;
    
    Move_t nextMove;
    nextMove.token = players[0];
    
    //Center of green zone
    Point_t center;
    center.row = 8;
    center.col = 5;
    
    for (int i = 0; i < possMoves.size(); i++) {
        Move_t currMove;
        currMove.token = players[0];
        currMove.destination = possMoves[i];
        
        //Check if move is legal or possible
        if (legalMove(players, currMove)) {
            if (dist(center, currMove.destination) < distFromCenter) {
                distFromCenter = dist(center, currMove.destination);
                nextMove.destination = currMove.destination;
            }
        }
    }
    
    return nextMove;
}

vector<Point_t> findSoldiers(vector<Token_t> players, vector<Point_t> possMoves) {
    vector<Point_t> soldierLocs;
    
    //Iterate through all possible moves and see if there is a soldier nearby
    for (int i = 0; i < possMoves.size(); i++) {
        for (int j = 1; j < players.size(); j++) {
            if (possMoves[i] == players[j].location) {
                soldierLocs.push_back(possMoves[i]);
            }
        }
    }
    
    return soldierLocs;
}

//If there are no soldiers that can be eaten, return value will have null point
Move_t eatSoldier(vector<Token_t> players, vector<Point_t> soldierLocs) {
    Move_t dest;
    dest.token = players[0];
    dest.destination.row = -1;
    
    double distFromCenter = INFINITY;   //If multiple soldiers can be jumped one closest to center will be jumped
    
    Point_t center;
    center.row = 8;
    center.col = 5;
    
    //Iterate through nearby soldier locations to see if they can be jumped
    for (int i = 0; i < soldierLocs.size(); i++) {
        //determine point behind soldier
        int rDiff = soldierLocs[i].row - players[0].location.row;
        int cDiff = soldierLocs[i].col - players[0].location.col;
        
        //Create temp move object for current jump being analyzed
        Point_t jumpPoint;
        jumpPoint.row = soldierLocs[i].row + rDiff;
        jumpPoint.col = soldierLocs[i].col + cDiff;
        
        Move_t jumpMove;
        jumpMove.token = players[0];
        jumpMove.destination = jumpPoint;
        
        //Check if move is legal/there are no soldiers at jump point
        if (legalMove(players, jumpMove) && dist(center, jumpMove.destination) < distFromCenter) {
            cout << "soldier can be jumped" << endl;
            distFromCenter = dist(center, jumpMove.destination);
            dest.destination = jumpMove.destination;
        }
    }
    
    return dest;
}

Move_t tigerStrategy(vector<Token_t> players, Color_t turn) {
    cout << "Custom tiger strategy 2.0" << endl;
    
    Move_t move;
    
    //Get all possible moves
    vector<Point_t> possMoves = getMoves(players);
    
    //Find any nearby soldiers
    vector<Point_t> soldierLocs = findSoldiers(players, possMoves);
        
    if (soldierLocs.size() > 0) {
        cout << "soldiers found" << endl;
        move = eatSoldier(players, soldierLocs);
    }
        
    if (move.destination.row == -1) {
        cout << "no soldiers found" << endl;
        move = nextMove(players, possMoves);
    }
    
    return move;
}

#endif
