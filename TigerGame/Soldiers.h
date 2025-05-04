#ifndef SDLTESTER_SOLDIERS_H
#define SDLTESTER_SOLDIERS_H

#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <queue>
#include <iostream>
#include "BearGame.h"
#include "constants.h"

using namespace std;

// Remember last soldier move to prevent immediate reversals
static Move_t lastSoldierMove = Move_t{ Token_t{BLUE, Point_t{-1,-1}}, Point_t{-1,-1} };

// game phases for soldiers
enum Phase {START, MID, CHECKMATE, SURVIVE};

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





//average row of all soldiers (lower is further up the board && better)
double meanSoldierRow(const vector<Token_t>& game) {
    double sum = 0;
    for (int i = 1; i < (int)game.size(); i++)
        sum += game[i].location.row;
    return (sum / (game.size() - 1));
}

int shortestPath(const vector<Token_t>& game, Point_t start, Point_t goal) {//TODO edit
    int rows = GRID_ROW, cols = GRID_COL;
    vector<vector<bool>> seen(rows, vector<bool>(cols,false));
    queue<pair<Point_t,int>> q;
    q.push({start,0});
    seen[start.row][start.col] = true;
    while (!q.empty()) {
        auto [pt, d] = q.front(); q.pop();
        if (pt == goal) return d;
        for (int dr=-1; dr<=1; ++dr) {
            for (int dc=-1; dc<=1; ++dc) {
                if (dr==0&&dc==0) continue;
                Point_t nxt{pt.row+dr, pt.col+dc};
                Move_t mv{ Token_t{game[0].color, pt}, nxt };
                // first guard against going off‐board:
                if (!validLocation(nxt))
                    continue;

                // now it's safe to index `seen`:
                if (!seen[nxt.row][nxt.col] && legalMove(game, mv)) {
                    seen[nxt.row][nxt.col] = true;
                    q.push({nxt, d+1});
                }

            }
        }
    }
    return INT_MAX; // unreachable
}

bool isAdjacent(Point_t a, Point_t b) {
    int dr = b.row - a.row;
    int dc = b.col - a.col;
    // orthogonal
    if (abs(dr) + abs(dc) == 1) return true;
    // diagonal
    if (abs(dr) == 1 && abs(dc) == 1) {
        auto f = make_pair(a,b), bkw = make_pair(b,a);
        return (find(diags.begin(), diags.end(), f) != diags.end())
               || (find(diags.begin(), diags.end(), bkw) != diags.end());
    }
    return false;
}

bool checkJumpable(const vector<Token_t>& game, int idx) {
    Point_t s = game[idx].location;

    // try every direction around the soldier
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;

            Point_t from   { s.row + dr,   s.col + dc   };  // where tiger must stand
            Point_t landing{ s.row - dr,   s.col - dc   };  // where tiger lands

            // 1) both squares must be on-board
            if (!validLocation(from) || !validLocation(landing))
                continue;

            // 2) those steps must be legal edges
            if (!isAdjacent(from, s) || !isAdjacent(s, landing))
                continue;

            // 3) landing must be unoccupied
            bool occupied = false;
            for (auto &t : game) {
                if (t.location == landing) { occupied = true; break; }
            }
            if (occupied) continue;

            // 4) can the tiger actually *reach* the from-square?
            if (shortestPath(game, game[0].location, from) < INT_MAX)
                return true;
        }
    }

    return false;
}

int countJumpable(const vector<Token_t>& g) {
    int cnt = 0;
    for (int i = 1; i < (int)g.size(); ++i)
        if (checkJumpable(g, i))
            ++cnt;
    return cnt;
}

bool isJumpable(const vector<Token_t>& game, int idx) {
    return checkJumpable(game, idx);
}

bool isSaveable(const vector<Token_t>& game, int idx) {//TODO Edit
    // if not jumpable at all, it's safe
    if (!checkJumpable(game, idx))
        return true;

    Point_t s = game[idx].location;
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

            // can the tiger actually get to 'from'?
            int tigerDist = shortestPath(game, game[0].location, from);
            if (tigerDist == INT_MAX)
                continue;

            // can *any* other soldier get to landing in time?
            int bestSoldierDist = INT_MAX;
            for (int j = 1; j < (int)game.size(); ++j) {
                if (j == idx) continue;
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


int countUnsavableJumpable(const vector<Token_t>& game) {//counts # of unsaveable soldiers after a move (less is better)
    int cnt = 0;
    for (int i = 1; i < (int)game.size(); ++i) {
        // use the boolean checkJumpable now
        if (checkJumpable(game, i) && !isSaveable(game, i))
            ++cnt;
    }
    return cnt;
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




Move_t bestOverallMove(
        const vector<Token_t>& game,
        double weightRegion,
        double weightCluster,
        double weightJump,
        double weightLocalRisk,
        double weightAdvance,
        double weightThreatDist
) {
    using ScoreMove = pair<double,Move_t>;
    auto cmp = [](const ScoreMove &a, const ScoreMove &b) {
        return a.first > b.first;  // min-heap
    };
    priority_queue<ScoreMove, vector<ScoreMove>, decltype(cmp)> pq(cmp);

    const int    MAX_UNSAVEABLE    = 2;
    const double WEIGHT_UNSAVEABLE = 10.0;

    for (auto &m : generateAllSoldierMoves(game)) {
        // simulate
        auto sim = game;
        int movedIdx = -1;
        for (int k = 1; k < (int)sim.size(); ++k) {
            if (sim[k].location == m.token.location) {
                sim[k].location = m.destination;
                movedIdx = k;
                break;
            }
        }

        int unsavable = countUnsavableJumpable(sim);
        if (unsavable > MAX_UNSAVEABLE) continue;

        double R = (double)tigerReachableRegion(sim);
        double C = (double)soldierClusters(sim);
        double J = (double)countJumpable(sim);
        double A = (double)meanSoldierRow(sim);
        double localRisk = (movedIdx>=0 && isJumpable(sim, movedIdx)) ? 1.0 : 0.0;

        double minD = 1e9;
        for (int i = 1; i < (int)sim.size(); ++i)
            minD = min(minD, dist(sim[0].location, sim[i].location));

        double score =
                weightRegion    * R
                + weightCluster   * C
                + weightJump      * J
                + weightLocalRisk * localRisk
                + weightAdvance   * A
                + weightThreatDist * (-minD)
                - WEIGHT_UNSAVEABLE * unsavable;

        // penalize immediate reverse
        if (lastSoldierMove.token.location == m.destination &&
            lastSoldierMove.destination == m.token.location) {
            score += 1e6;
        }

        pq.emplace(score, m);
    }

    if (pq.empty()) {
        cout << "pq empty in bestOverallMove\n";
        //return randomMove_soldiers(game);
    }

    ScoreMove best = pq.top();
    lastSoldierMove = best.second;
    return best.second;
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






Move_t Move_NoahsNaiveNights_Men(vector<Token_t> game, Color_t turn) {
    if (turn != BLUE)
        return randomMove_soldiers(game);

    int exposedCount = countJumpable(game);
    if (exposedCount > 0) {
        // Protect Mode
        return bestOverallMove(game, 5.0, 2.0, 4.0, 10.0, 0.5, 2.0);
    }

    // March Mode
    Phase p = getPhase(game);
    double minDist = 1e9;
    for (int i = 1; i < (int)game.size(); ++i)
        minDist = min(minDist, dist(game[0].location, game[i].location));
    bool threat = (minDist < 2.0);

    switch (p) {
        case START:
            return bestOverallMove(game, 4.0, 1.0, 1.0, 1.0, 8.0, 0.0);
        case MID:
            return bestOverallMove(game, 3.5, 1.0, 1.0, 1.0, 6.0, threat?2.0:0.0);
        case CHECKMATE:
            return bestOverallMove(game, 2.0, 0.5, 0.5, 0.5, 4.0, 0.0);
        default:
            return randomMove_soldiers(game);
    }
}




#endif //SDLTESTER_SOLDIERS_H
