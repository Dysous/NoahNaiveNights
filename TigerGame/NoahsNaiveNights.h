/**
 * Authors: Noah Mathew, Nicholas Storti, Anjan Godavarti, Zach Taylor
 * Assignment Title: Group Project - Tiger Game
 * Assignment Description: Implementation of Move function
 *     for group project Tiger Game
 * Due Date: 05/04/2025
 * Date Created: 04/13/2025
 * Date Last Modified: 05/04/2025
 */

#ifndef NOAHSNAIVENIGHTS_H
#define NOAHSNAIVENIGHTS_H

#include <queue>
#include <iostream>
#include "BearGame.h"
#include "constants.h"
#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

using namespace std;


// Remember last soldier move to prevent undoing the last move
static Move_t lastSoldierMove = Move_t{ Token_t{BLUE, Point_t{-1,-1}},
                                                      Point_t{-1,-1} };

// game phases for soldiers
enum Phase {START, MID, CHECKMATE, SURVIVE};

// previous phase
static Phase lastPhase;

// diagonal edges
static vector<pair<Point_t,Point_t>> diags = {
        // lair
        {Point_t(0,4), {1,3}}, {Point_t(0,4), {1,5}},
        {Point_t(1,3), {2,2}}, {Point_t(1,3), {2,4}},
        {Point_t(1,5), {2,4}}, {Point_t(1,5), {2,6}},
        {Point_t(2,2), {3,3}}, {Point_t(2,4), {3,3}},
        {Point_t(2,4), {3,5}}, {Point_t(2,6), {3,5}},
        {Point_t(3,3), {4,4}}, {Point_t(3,5), {4,4}},
        // rest of board
        {Point_t(4,4), {5,3}}, {Point_t(4,4), {5,5}},
        {Point_t(5,3), {6,2}}, {Point_t(5,5), {6,6}},
        {Point_t(6,2), {7,1}}, {Point_t(6,6), {7,7}},
        {Point_t(7,1), {8,0}}, {Point_t(7,7), {8,8}},
        {Point_t(8,0), {9,1}}, {Point_t(8,8), {9,7}},
        {Point_t(9,1), {10,2}}, {Point_t(9,7), {10,6}},
        {Point_t(10,2), {11,3}}, {Point_t(10,6), {11,5}},
        {Point_t(11,3), {12,4}}, {Point_t(11,5), {12,4}},
};

//HELPERS:

/**
 * description: check if location on grid
 * return: bool
 * precondition: point is passed of player location
 * postcondition: true if it is a valid location,
 *     false otherwise
 */
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

/**
 * description: check if move is legal
 * return: bool
 * precondition: current game state and potential move
 *     are passed
 * postcondition: given current game state, returns true
 *     if the proposed move is legal, false otherwise
 */
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
        ok = (find(diags.begin(), diags.end(), forward ) != diags.end())
          || (find(diags.begin(), diags.end(), backward) != diags.end());
    }
    if (!ok) return false;

    // check if in lion's den
    //do we want to keep this? this just disallows moving to top diamond?
    /*
    if (move.destination.row < 4 &&
       (move.token.location.row == move.destination.row ||
        move.token.location.col == move.destination.col)) {
        return false;
    }
    */
    // check if interfering with other player
    for (auto i = game.begin(); i != game.end(); i++) {
        if (move.destination == i->location) {
            return false;
        }
    }

    return true;
}

/**
 * description: geometric distance between two points
 * return: double
 * precondition: two points passed as arguments
 * postcondition: geometric distance between the points
 *     returned
 */
double dist(Point_t p1, Point_t p2) {
    return sqrt((p1.row - p2.row) * (p1.row - p2.row) + (
                 p1.col - p2.col) * (p1.col - p2.col));
}


//TIGER:

// game phases for soldiers
enum Zones {REDZONE, YELLOWZONE, GREENZONE};

// comparison class for point objects
struct comparePoints{
    /**
     * description: comparison function for points
     * return: bool
     * precondition: two points passed by reference
     * postcondition: true if point a less than b, false otherwise
     */
    bool operator()(const Point_t& a, const Point_t& b) const{
        return a.row < b.row or (a.row == b.row and a.col < b.col);
    }
};

/**
 * description: Hard codes each of the tigers zones and returns
 *     what zone the tiger is currently in
 * return: enumerated zone type
 * precondition: location of player passed by reference as a point
 * postcondition: zone of the location returned
 */
Zones getCurZone(Point_t& location){
    cout << "Getting zone" << endl;
    //probably more efficent way of doing this but leaving it for now
    const set<Point_t,comparePoints> redzone = {
            //Top Tiger nest
            {0,4},{1,3},{1,5},{2,2},{2,4},{2,6},{3,3},{3,5},
            //top right corner
            {4,0},{4,1},{5,0},
            //top left corner
            {4,7},{4,8},{5,8},
            //bottom right corner
            {11,0},{12,1},{12,0},
            //bottom left corner
            {12,7},{12,8},{11,8}

    };
    const set<Point_t,comparePoints> greenzone = {
            {6,2}, {6,3},{6,4},{6,5}, {6,6},
            {7,2}, {7,3}, {7,4}, {7,5}, {7,6},
            {8,2}, {8,3}, {8,4}, {8,5}, {8,6},
            {9,2}, {9,3}, {9,4}, {9,5}, {9,6},
            {10,2}, {10,3},{10,4},{10,5}, {10,6}
    };

    if(redzone.find(location) != redzone.end()){
        return REDZONE;
    }
    else if(greenzone.find(location) != greenzone.end()){
        return GREENZONE;
    }
    else{
        return YELLOWZONE;
    }
}

/**
 * description: finds possible moves for tiger
 * return: vector of points
 * precondition: current game state passed as an argument
 * postcondition: vector of points representing possible tiger moves
 *     returned
 */
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

/**
 * description: possible next move for tiger
 * return: move object
 * precondtion: game state and possible moves passed as arguments
 * postcondition: returns optimal move for tiger
 */
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

/**
 * description: finds positions of soldiers
 * return: vector of points
 * precondition: current game state and possible moves passed
 * postcondition: returns points where soldiers are located
 */
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

/**
 * description: move for jumping a soldier
 * return: move
 * precondition: game state and soldier locations passed
 * postcondition: tries to eat a soldier. If there are no soldiers
 *     that can be eaten, return value will have null point
 */
Move_t eatSoldier(vector<Token_t> players, vector<Point_t> soldierLocs) {
    Move_t dest;
    dest.token = players[0];
    dest.destination.row = -1;

    double distFromCenter = INFINITY;
    //If multiple soldiers can be jumped one closest to center will be jumped

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
        if (legalMove(players, jumpMove) &&
            dist(center, jumpMove.destination) < distFromCenter) {
            distFromCenter = dist(center, jumpMove.destination);
            dest.destination = jumpMove.destination;
        }
    }

    return dest;
}

/**
 * description: tiger move function
 * return: move
 * precondition: game state and turn passed
 * postcondition: returns move for the tiger
 */
Move_t tigerStrategy(vector<Token_t> players, Color_t turn) {
    cout << "Custom tiger strategy" << endl;

    Move_t move;

    //Get all possible moves
    vector<Point_t> possMoves = getMoves(players);

    if (getCurZone(players[0].location) != REDZONE) {
        //Find any nearby soldiers
        vector<Point_t> soldierLocs = findSoldiers(players, possMoves);

        if (soldierLocs.size() > 0) {
            move = eatSoldier(players, soldierLocs);
        }

        if (move.destination.row == -1) {
            move = nextMove(players, possMoves);
        }

    } else {
        move = nextMove(players, possMoves);
    }

    return move;
}

/**
 * description: returns point where tiger could jump to over given man
 * return: point
 * precondition: game state and soldier index
 * postcondition: point where tiger could jump over given soldier.
 *     Returns (-1, -1) if no point exists
 */
Point_t checkJump(vector<Token_t> game, int man) {
    Point_t jmp(-1,-1);

    // first check if tiger is in range
    if (dist(game[0].location, game[man].location) < 2) {
        // check points surrounding man
        if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row, game[man].location.col + 1)))) {
            jmp = Point_t(game[man].location.row, game[man].location.col + 1);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row, game[man].location.col - 1)))) {
            jmp = Point_t(game[man].location.row, game[man].location.col - 1);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row + 1, game[man].location.col)))) {
            jmp = Point_t(game[man].location.row + 1, game[man].location.col);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row - 1, game[man].location.col)))) {
            jmp = Point_t(game[man].location.row - 1, game[man].location.col);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row + 1, game[man].location.col + 1)))) {
            jmp = Point_t(game[man].location.row + 1, game[man].location.col + 1);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row + 1, game[man].location.col - 1)))) {
            jmp = Point_t(game[man].location.row + 1, game[man].location.col - 1);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row - 1, game[man].location.col + 1)))) {
            jmp = Point_t(game[man].location.row - 1, game[man].location.col + 1);
        }
        else if (legalMove(game, Move_t(game[0],
            Point_t(game[man].location.row - 1, game[man].location.col - 1)))) {
            jmp = Point_t(game[man].location.row - 1, game[man].location.col - 1);
        }
    }

    return jmp;
}

/**
 * description: generates all possible soldier moves
 * return: vector of moves
 * precondition: current game state passed as argument
 * postcondition: vector of moves representing soldier moves
 *     returned
 */
vector<Move_t> generateAllSoldierMoves(const vector<Token_t>& game) {
    vector<Move_t> moves;
    for (size_t i = 1; i < game.size(); i++) {
        Point_t source = game[i].location;
        for (int destRow = -1; destRow <= 1; destRow++) {
            for (int destCol = -1; destCol <= 1; destCol++) {
                if (destRow == 0 && destCol == 0){
                    continue;
                }
                Point_t dest{ source.row + destRow, source.col + destCol };
                Move_t m{ game[i], dest };
                if (legalMove(game, m))
                    moves.push_back(m);
            }
        }
    }
    return moves;
}

/**
 * description: find neighboring soldiers to given soldier
 * return: vector of integers
 * precondition: game state and index of given soldier passed
 * postcondition: returns indices of soldiers neighboring given
 *     soldier
 */
vector<int> getSoldierNeighbors(const vector<Token_t>& game, int n) {
    vector<int> adj;
    Point_t a = game[n].location;
    for (int j = 1; j < (int)game.size(); j++) {
        if (j == n) continue;
        Point_t b = game[j].location;

        int dr = abs(a.row - b.row), dc = abs(a.col - b.col);
        bool orth = (dr + dc) == 1;
        bool diag = dr == 1 && dc == 1 &&
                    (find(diags.begin(), diags.end(),
                          make_pair(a,b)) != diags.end() ||
                     find(diags.begin(), diags.end(),
                          make_pair(b,a)) != diags.end());

        if (orth || diag)
            adj.push_back(j);
    }
    return adj;
}

/**
 * description: measure of soldier distance from each other
 * return: integer
 * precondition: game state passed
 * postcondition: soldier distance magnitude returned
 */
int absSoldierDist(const vector<Token_t>& game) {
    int best = 0;
    for (size_t i = 1; i < game.size(); i++) {
        for (size_t j = i+1; j < game.size(); j++) {
            int destRow = abs(game[i].location.row - game[j].location.row);
            int destCol = abs(game[i].location.col - game[j].location.col);
            best = max(best, destRow+destCol);
        }
    }
    return best;
}

/**
 * description: average row of all soldiers. Lower is further
 *     up the board and considered better.
 * return: double
 * precondition: current game state passed
 * postcondition: average row of soldiers returned
 */
double meanSoldierRow(const vector<Token_t>& game) {
    double sum = 0;
    for (int i = 1; i < (int)game.size(); i++)
        sum += game[i].location.row;
    return (sum / (game.size() - 1));
}

/**
 * description: shortest path to reach a point from given start point
 * return: integer
 * precondition: current game state, start point, and end point passed
 * postcondition: integer distance of shortest path from start to end
 *     returned
 */
int shortestPath(const vector<Token_t>& game, Point_t start, Point_t goal) {
    int rows = GRID_ROW, cols = GRID_COL;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    queue<pair<Point_t,int>> q;
    q.push({start, 0});
    visited[start.row][start.col] = true;

    while (!q.empty()) {
        auto [pt, d] = q.front();
        q.pop();

        if (pt == goal){
            return d;

        }

        //all legal one step moves
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0)
                    continue;
                Point_t next{pt.row + dr, pt.col + dc};
                Move_t mv{ Token_t{game[0].color, pt}, next };

                if (!validLocation(next))
                    continue;
                if (visited[next.row][next.col])
                    continue;
                if (!legalMove(game, mv))
                    continue;

                visited[next.row][next.col] = true;
                q.push({next, d + 1});
            }
        }

        // all legal jumps over an soldier
        //causing errors
        /*
        for (int i = 1; i < (int)game.size(); ++i) {
            Point_t soldier = game[i].location;
            int dr = soldier.row - pt.row;
            int dc = soldier.col - pt.col;

            // must be orthogonal or a legal diagonal
            bool orth = (abs(dr) + abs(dc) == 1);
            bool diag = (abs(dr) == 1 && abs(dc) == 1 &&
                         (find(diags.begin(), diags.end(),
                               make_pair(pt, soldier)) != diags.end()));

            if (!(orth || diag))
                continue;

            //one more step in same direction for jump
            Point_t land{ soldier.row + dr, soldier.col + dc };
            if (!validLocation(land) || visited[land.row][land.col])
                continue;

            //landing dest must be empty
            bool occupied = false;
            for (auto &t : game) {
                if (t.location == land) {
                    occupied = true;
                    break;
                }
            }
            if (occupied)
                continue;

            visited[land.row][land.col] = true;
            q.push({land, d + 1});
        }
        */
    }

    return INT_MAX;//unreachable
}

/**
 * description: checks if point is adjacent
 * return: bool
 * precondition: two points to check adjacency passed
 * postcondition: true if adjacent, false otherwise
 */
bool isAdjacent(Point_t a, Point_t b) {
    int dr = b.row - a.row;
    int dc = b.col - a.col;
    // orthogonal
    if (abs(dr) + abs(dc) == 1) {
        return true;
    }
    // diagonal
    if (abs(dr) == 1 && abs(dc) == 1) {
        auto f = make_pair(a,b), bkw = make_pair(b,a);
        return (find(diags.begin(), diags.end(), f) !=
                diags.end())|| (find(diags.begin(), diags.end(),
                                     bkw) != diags.end());
    }
    return false;
}

/**
 * description: check if given soldier is jumpable
 * return: bool
 * precondition: current game state and index of given soldier passed
 * postcondition: true if soldier is jumpable, false otherwise
 */
bool checkJumpable(const vector<Token_t>& game, int n) {
    Point_t s = game[n].location;

    // try every direction around the soldier
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            Point_t from   { s.row + dr,   s.col + dc   };  // where tiger must stand
            Point_t landing{ s.row - dr,   s.col - dc   };  // where tiger lands

            //both squares must be on-board
            if (!validLocation(from) || !validLocation(landing))
                continue;

            //those steps must be legal edges
            if (!isAdjacent(from, s) || !isAdjacent(s, landing))
                continue;

            //landing must be unoccupied
            bool occupied = false;
            for (auto &t : game) {
                if (t.location == landing) { occupied = true; break; }
            }
            if (occupied) continue;

            //can the tiger actually reach dest square of jump
            if (shortestPath(game, game[0].location, from) < INT_MAX)
                return true;
        }
    }

    return false;
}

/**
 * description: number of jumpable soldiers
 * return: integer
 * precondition: current game state
 * postcondition: number of jumpable soldiers
 */
int countJumpable(const vector<Token_t>& g) {
    int count = 0;
    for (int i = 1; i < (int)g.size(); i++)
        if (checkJumpable(g, i))
            count++;
    return count;
}

/**
 * description: wrapper function to check jumpability
 * return: bool
 * precondition: game state and soldier index
 * postcondition: true if soldier is jumpable, false otherwise
 */
bool isJumpable(const vector<Token_t>& game, int n) {
    return checkJumpable(game, n);
}

/**
 * description: check if soldier can be saved
 * return: bool
 * precondition: game state and soldier index
 * postcondition: true if he can be saved, false otherwise
 */
bool isSaveable(const vector<Token_t>& game, int n) {
    // if not jumpable its safe
    if (!checkJumpable(game, n))
        return true;

    Point_t s = game[n].location;
    // look in every direction for a potential "from" and "landing"
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;

            Point_t from{   s.row + dr,   s.col + dc   };
            Point_t landing{s.row - dr, s.col - dc};

            // both squares must be on‐board
            if (!validLocation(from) || !validLocation(landing))
                continue;

            // landing must be a legal tiger move
            if (!legalMove(game, Move_t{game[0], landing}))
                continue;

            // can the tiger actually get to from?
            int tigerDist = shortestPath(game, game[0].location, from);
            if (tigerDist == INT_MAX)
                continue;

            // can *any* other soldier get to landing in time?
            int bestSoldierDist = INT_MAX;
            for (int j = 1; j < (int)game.size(); ++j) {
                if (j == n) continue;
                bestSoldierDist = min(
                        bestSoldierDist,
                        shortestPath(game, game[j].location, landing)
                );
            }

            // if no soldier can beat or tie the tiger, this hole is unsaveable
            if (bestSoldierDist > tigerDist)
                return false;
        }
    }

    // every possible hole was reachable by a backup soldier first
    return true;
}

/**
 * description: counts number of unsaveable soldiers after
 *     a move
 * return: integer
 * precondition: game state
 * postcondition: number of unsaveable soldiers returned
 */
int countUnsavableJumpable(const vector<Token_t>& game) {
    int count = 0;
    for (int i = 1; i < (int)game.size(); ++i) {
        // use the boolean checkJumpable now
        if (checkJumpable(game, i) && !isSaveable(game, i))
            count++;
    }
    return count;
}


//WEIGHTED FACTORS:

/**
 * description: gives weighting representing how soldiers
 *     are clustered
 * return: integer
 * precondition: game state passed
 * postcondition: number representing soldier clustering returned
 */
int soldierClusters(const vector<Token_t>& game) {
    int clusters = 0;
    int n = game.size();
    vector<bool> seen(n, false);

    //only ids 1 to n-1 are soldiers
    for (int id = 1; id < n; id++) {
        if (seen[id]) continue;
        clusters++;
        queue<int> bfs;
        bfs.push(id);
        seen[id] = true;

        while (!bfs.empty()) {
            int u = bfs.front(); bfs.pop();
            for (int v : getSoldierNeighbors(game, u)) {
                if (!seen[v]) {
                    seen[v] = true;
                    bfs.push(v);
                }
            }
        }
    }

    return clusters;
}

/**
 * description: checks if soldier currently jumpable
 * return: bool
 * precondition: game state and soldier index passed
 * postcondition: true if soldier can be jumped, false otherwise
 */
bool immediateJumpable(const vector<Token_t>& game, int n) {
    Point_t p    = game[n].location;
    Point_t tpos = game[0].location;
    // compute vector from tiger to soldier
    int dr = p.row - tpos.row;
    int dc = p.col - tpos.col;

    // must be adjacent (orthogonal or one of your legal diagonals)
    bool orth =  (abs(dr) + abs(dc) == 1);
    bool diag =  (abs(dr) == 1 && abs(dc) == 1 &&
                  (find(diags.begin(), diags.end(),
                        make_pair(tpos, p)) != diags.end()));
    if (!(orth || diag))
        return false;

    // landing spot beyond soldier
    Point_t landing{ p.row + dr, p.col + dc };
    if (!validLocation(landing))
        return false;

    // no one sitting on the landing square
    for (auto &tk : game)
        if (tk.location == landing)
            return false;

    // can the tiger legally move from its current square to p?
    if (!legalMove(game, Move_t{game[0], p}))
        return false;

    // can it then legally jump from p to landing?
    if (!legalMove(game, Move_t{ Token_t{game[0].color, p}, landing }))
        return false;

    return true;
}

//PRIORITY Qs
/**
 * description: checks where tiger can reach
 * return: integer
 * precondition: game state
 * postcondition: returns integer representing tiger reachable
 *     region
 */
int tigerReachableRegion(const vector<Token_t>& game) {
    //bfs over the adj grid
    //occupied squares should block
    bool seen[GRID_ROW][GRID_COL] = {};
    queue<Point_t> q;

    Point_t start = game[0].location;
    seen[start.row][start.col] = true;
    q.push(start);

    int count = 1;
    while (!q.empty()) {
        auto current = q.front(); q.pop();

        for (int destRow = -1; destRow <= 1; destRow++) {
            for (int destCol = -1; destCol <= 1; destCol++) {
                if (destRow==0 && destCol==0) continue;
                // inside the for(destRow, destCol) loops:
                Point_t next{current.row+destRow, current.col+destCol};

                // must check bounds before touching seen[][]:
                if (!validLocation(next))
                    continue;

                Move_t move{ Token_t{game[0].color, current}, next };
                if (!seen[next.row][next.col] && legalMove(game, move)) {
                    seen[next.row][next.col] = true;
                    q.push(next);
                    count++;
                }

            }
        }
    }
    return count;
}


//MOVE FUNCTIONS:

/**
 * description: determine game phase
 * return: enumerated phase type
 * precondition: given game state and current phase
 * postcondition: returns game phase determined from game state
 */
Phase getPhase(vector<Token_t> game, Phase current) {
    int soldiers = game.size() - 1;
    if (soldiers <= 1) return SURVIVE;
    int region = tigerReachableRegion(game);
    if (region > 35 && current != MID) return START;
    if (region <= 35 && region > 10) return MID;
    return CHECKMATE;
}

/**
 * description: generate random move for soldiers
 * return: move
 * precondition: game state passed
 * postcondition: returns random move for a solider
 */
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


/**
 * description: minimum soldier distance from tiger
 * return: double
 * precondition: game phase and potential move passed
 * postcondition: returns minimum distance from a soldier to tiger
 */
static double minSoldierDistFromTiger(const vector<Token_t>& base, const Move_t &m) {
    auto sim = base;
    for (int i = 1; i < (int)sim.size(); ++i) {
        if (sim[i].location == m.token.location) {
            sim[i].location = m.destination;
            break;
        }
    }
    double best = 1e9;
    for (int i = 1; i < (int)sim.size(); ++i)
        best = min(best, dist(sim[0].location, sim[i].location));
    return best;
}


/**
 * description: generate move for starting phase
 * return: move
 * precondition: game state passed
 * postcondition: returns move for starting phase of the game
 */
Move_t startPhaseMove(const vector<Token_t>& game) {
    const int MAX_JUMPABLE_BEFORE = 3;
    const int MAX_ROW_DIFF       = 1;

    //current vulnerability and back rank row
    int currJumpable = countJumpable(game);
    int backRow = 0;
    for (int i = 1; i < (int)game.size(); ++i) {
        backRow = max(backRow, game[i].location.row);
    }
    struct Cand {
        Move_t m;
        double safety;
    };

    vector<Cand> safeCands;
    vector<Move_t> allForwards;

    //all forward moves w/ filters
    for (int i = 1; i < (int)game.size(); ++i) {
        int srcRow = game[i].location.row;

        //skip move if already exposed - commented out for higher risk/success
        //if (checkJumpable(game, i))
        //continue;
        //skip  move if too far ahead of back rank
        if ((backRow - srcRow) > MAX_ROW_DIFF)
            continue;

        Point_t dest{ srcRow - 1, game[i].location.col };
        Move_t m{ game[i], dest };
        if (!legalMove(game, m))
            continue;

        //sim the forward step
        auto sim = game;
        sim[i].location = dest;

        //skip move if this soldier is now immediately jumpable next turn
        if (immediateJumpable(sim, i))
            continue;

        //skip move if total jumpable would exceed the set limit
        int newJumpable = countJumpable(sim);
        if (currJumpable >= MAX_JUMPABLE_BEFORE && newJumpable > currJumpable)
            continue;

        allForwards.push_back(m);

        //best moves, only keep those a backup soldier can save
        if (isSaveable(sim, i)) {
            double minD = 1e9;
            for (int j = 1; j < (int)sim.size(); ++j)
                minD = min(minD, dist(sim[0].location, sim[j].location));
            safeCands.push_back({m, minD});
        }
    }

    //prefer a forward step that’s saveable move
    if (!safeCands.empty()) {
        sort(safeCands.begin(), safeCands.end(), [&](auto &A, auto &B){
            if (A.m.destination.row != B.m.destination.row)
                return A.m.destination.row < B.m.destination.row;
            return A.safety > B.safety;
        });
        return safeCands.front().m;
    }

    //otherwise any forward step as long as its not immediately jumpable
    if (!allForwards.empty()) {
        sort(allForwards.begin(), allForwards.end(), [&](auto &a, auto &b){
            if (a.destination.row != b.destination.row)
                return a.destination.row < b.destination.row;
            double da = minSoldierDistFromTiger(game, a);
            double db = minSoldierDistFromTiger(game, b);
            return da > db;
        });
        return allForwards.front();
    }

    //no safe moves
    cout << "NO SAFE MOVE" << endl;
    return Move_t{ Token_t{BLUE, Point_t{-1,-1}}, Point_t{-1,-1} };
}

/**
 * description: move for survival phase of the game
 * return: move
 * precondition: game state passed
 * postcondition: gives move if in the survival phase
 *     of the game
 */
Move_t survivePhaseMove(const vector<Token_t>& game) {
    int n = game.size() - 1;
    // If only one soldier, stick to walls
    if (n == 1) {
        Token_t sol = game[1];
        vector<Move_t> options;
        for (int dr=-1; dr<=1; dr++) for (int dc=-1; dc<=1; dc++) {
                if (!dr && !dc) continue;
                Point_t dest{sol.location.row+dr, sol.location.col+dc};
                Move_t m{sol, dest};
                if (legalMove(game, m)) options.push_back(m);
            }
        vector<Move_t> border;
        for (Move_t &m: options) {
            Point_t &p = m.destination;
            if (p.row==0 || p.row==GRID_ROW-1 || p.col==0 || p.col==GRID_COL-1)
                border.push_back(m);
        }
        vector<Move_t> &cand = border.empty() ? options : border;
        double bd=-1; Move_t best = cand.front();
        for (Move_t &m: cand) {
            double d = dist(game[0].location, m.destination);
            if (d > bd) { bd=d; best=m; }
        }
        return best;
    }
    // Multiple soldiers: maximize minimum distance
    auto moves = generateAllSoldierMoves(game);
    double bestMin=-1;
    Move_t bestMove = moves.front();
    for (auto &m: moves) {
        auto sim = game;
        for (int i=1; i<(int)sim.size(); ++i)
            if (sim[i].location==m.token.location) { sim[i].location=m.destination; break; }
        double mn = numeric_limits<double>::infinity();
        for (int i=1; i<(int)sim.size(); ++i)
            mn = min(mn, dist(sim[0].location, sim[i].location));
        if (mn > bestMin) { bestMin=mn; bestMove=m; }
    }
    return bestMove;
}

/**
 * description: move for the soldiers
 * return: move
 * precondition: game phase and turn passed
 * postcondition: returns move for soldiers
 */
Move_t Move_NoahsNaiveNights_Men(vector<Token_t> game, Color_t turn) {
    if (turn != BLUE)
        return randomMove_soldiers(game);

    int exposedCount = countJumpable(game);
    if (exposedCount > 0) {
        // Protect Mode
    }

    // March Mode
    Phase p = getPhase(game, lastPhase);
    lastPhase = p;
    double minDist = 1e9;
    for (int i = 1; i < (int)game.size(); ++i)
        minDist = min(minDist, dist(game[0].location, game[i].location));
    bool threat = (minDist < 2.0);

    switch (p) {
        case START:
            cout << "START" << endl;
            return startPhaseMove(game);
        case SURVIVE:
            cout << "SURVIVE" <<endl;
            return survivePhaseMove(game);
        case CHECKMATE:
            cout << "END" << endl;
            //return
        default:
            return randomMove_soldiers(game);
    }
}


/**
 * description: move function for tiger game
 * return: move
 * precondition: game phase and turn given
 * postcondition: gives move for soldiers or men depending on turn
 */
Move_t Move_NoahsNaiveNights(vector<Token_t> game, Color_t turn){
    if(turn == BLUE){
        return Move_NoahsNaiveNights_Men(game,turn);
    }
    else{
        return tigerStrategy(game,turn);
    }
}


#endif //NOAHSNAIVENIGHTS_H
