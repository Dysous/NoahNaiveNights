
#ifndef SDLTESTER_SOLDIERS_H
#define SDLTESTER_SOLDIERS_H



#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "BearGame.h"
#include "constants.h"

using namespace std;

// game phases for soldiers
enum Phase {START, MID, CHECKMATE, SURVIVE};

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

//HELPERS:

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

    // check if in lion's den
    //do we want to keep this? this just disallows moving to top diamond?
    /*
    if (move.destination.row < 4 && (move.token.location.row == move.destination.row ||
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

vector<Move_t> generateAllSoldierMoves(const vector<Token_t>& game) {
    vector<Move_t> moves;
    for (size_t i = 1; i < game.size(); i++) {
        Point_t source = game[i].location;
        for (int destRow = -1; destRow <= 1; destRow++) {
            for (int destCol = -1; destCol <= 1; destCol++) {
                if (destRow == 0 && destCol == 0) continue;
                Point_t dst{ source.row + destRow, source.col + destCol };
                Move_t m{ game[i], dst };
                if (legalMove(game, m))
                    moves.push_back(m);
            }
        }
    }
    return moves;
}

vector<int> getSoldierNeighbors(const vector<Token_t>& game, int idx) {
    vector<int> adj;
    Point_t a = game[idx].location;
    for (int j = 1; j < (int)game.size(); j++) {
        if (j == idx) continue;
        Point_t b = game[j].location;

        int dr = abs(a.row - b.row), dc = abs(a.col - b.col);
        bool orth = (dr + dc) == 1;
        bool diag = dr == 1 && dc == 1 &&
                    (find(diags.begin(), diags.end(), make_pair(a,b)) != diags.end() ||
                     find(diags.begin(), diags.end(), make_pair(b,a)) != diags.end());

        if (orth || diag)
            adj.push_back(j);
    }
    return adj;
}

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


//WEIGHTED FACTORS:


int tigerMobilityScore(const vector<Token_t>& game) {
    int count = 0;
    for (int destRow = -1; destRow <= 1; destRow++) {
        for (int destCol = -1; destCol <= 1; destCol++) {
            if (destRow == 0 && destCol == 0) continue;
            Point_t dest{ game[0].location.row + destRow,
                          game[0].location.col + destCol };
            Move_t m{ game[0], dest };
            if (legalMove(game, m)) count++;
        }
    }
    return count;
}


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
//PRIORITY Qs

Move_t bestMoveByTigerMobility(const vector<Token_t>& game) {
    auto moves = generateAllSoldierMoves(game);

    //comparator:
    auto cmp = [](const pair<int, Move_t>& a, const pair<int, Move_t>& b) {
        return a.first > b.first;
    };

    // min heap by mobility # count
    priority_queue<pair<int, Move_t>, vector<pair<int, Move_t>>, decltype(cmp)> pq(cmp);

    //sim moves and see which move is best by mobility
    for (auto &m : moves) {
        auto sim = game;
        //find which soldier moved
        for (size_t k = 1; k < sim.size(); k++) {
            if (sim[k].location == m.token.location) {
                sim[k].location = m.destination;
                break;
            }
        }
        int score = tigerMobilityScore(sim);
        pq.emplace(score, m);
    }
    return pq.top().second;
}

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
                Point_t next{current.row+destRow, current.col+destCol};
                Move_t move{ Token_t{game[0].color, current}, next };
                // only step if legal (blocks on soldiers) and not seen
                if (!seen[next.row][next.col]
                    && legalMove(game, move)) {
                    seen[next.row][next.col] = true;
                    q.push(next);
                    count++;
                }
            }
        }
    }
    return count;
}


Move_t bestByConnectivity(const vector<Token_t>& game) {

    auto moves = generateAllSoldierMoves(game);

    // comparator: smaller moves with smaller cluster count are higher priority
    auto cmp = [](const pair<int,Move_t>& a, const pair<int,Move_t>& b) {
        return a.first > b.first; //min heap
    };

    //min heap for moves by fewest number of clusters = higher priority
    priority_queue<pair<int,Move_t>,vector<pair<int,Move_t>>,decltype(cmp)> pq(cmp);
    for (auto &m : moves) {
        auto sim = game;
        for (size_t k = 1; k < sim.size(); k++) {
            if (sim[k].location == m.token.location) {
                sim[k].location = m.destination;
                break;
            }
        }
        int comps = soldierClusters(sim);

        pq.push({comps, m});
    }
    return pq.top().second;
}

Move_t bestByRegionAndDistance(const vector<Token_t>& game, double weightMobility, double weightConnectivity)
{
    auto moves = generateAllSoldierMoves(game);

    auto cmp = [](const pair<double,Move_t>& a,
                  const pair<double,Move_t>& b) {
        return a.first > b.first;
    };
    priority_queue<pair<double,Move_t>,vector<pair<double,Move_t>>,decltype(cmp)> pq(cmp);

    for (auto &m : moves) {
        // simulate
        auto sim = game;
        for (size_t k = 1; k < sim.size(); k++) {
            if (sim[k].location == m.token.location) {
                sim[k].location = m.destination;
                break;
            }
        }

        double R = (double)tigerReachableRegion(sim);
        double D = (double)absSoldierDist(sim);
        double score = weightMobility*R + weightConnectivity*D;

        pq.push({score, m});
    }
    return pq.top().second;
}



//MOVE FUNCTIONS:

// function to determine phase of game
// can refine definition later
// just doing start and middle for now
Phase getPhase(vector<Token_t> game) {
    int count = 0;
    Phase p;

    // defining start as > 3 soldiers still in back row
    for (int i = 1; i < game.size(); i++) {
        if (game[i].location.row == 11) {
            count++;
        }
    }
    if (count > 5) {
        p = START;
    }
    else { p = MID;}

    if (tigerReachableRegion(game) < 10){//if in a small area try to win?
        p = CHECKMATE;
    }
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

    if (turn !=  BLUE)
        return randomMove_soldiers(game);

    int tiger = 0;
    Phase p = getPhase(game);

    double minDist = 1e9;
    for (size_t i = 1; i < game.size(); i++)
        minDist = min(minDist, dist(game[0].location, game[i].location));
    bool threat = (minDist < 2.0);

    switch (p) {
        // want to try to move pieces to center while sticking together
        case START:
            cout << "START" << endl;
            return bestByRegionAndDistance(game, 0.1, 0.9);
        case MID:
            if (threat){
                cout << "MID THREAT" << endl;
                return bestByRegionAndDistance(game, 0.1, 0.9);
            }
            else{
                cout << "MID" << endl;
                return bestByRegionAndDistance(game, 0.1, .9);
            }
        case CHECKMATE:
            return bestMoveByTigerMobility(game);
        case SURVIVE:
            break;
    }
    return randomMove_soldiers(game);

}



#endif //SDLTESTER_SOLDIERS_H
