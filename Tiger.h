//
//  Tiger.h
//  CSI 3344 Group Project
//
//  Created by Zach Taylor on 4/30/25.
//

#ifndef Tiger_h
#define Tiger_h

#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "BearGame.h"
#include "constants.h"

using namespace std;

// game phases for soldiers
enum Zones {REDZONE, YELLOWZONE, GREENZONE};

struct comparePoints{
    bool operator()(const Point_t& a, const Point_t& b) const{
        return a.row < b.row or (a.row == b.row and a.col < b.col);
    }
};

Zones getCurZone(Point_t& location){
    //Hard codes each of the tigers zones and returns what zone the tiger is currently in
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

//Function to help generate all possible moves that the tiger can do
//Helper
vector<Move_t> possibleTigerMoves(const vector<Token_t>& game){
    vector<Move_t> moves;
    Point_t curTigLoc = game[0].location;
    
    for(int destRow = -2; destRow<=2; destRow++){
        for(int destCol = -2; destCol<=2; destCol++){
            if(destRow==0 and destCol ==0)continue;
            Point_t dst{curTigLoc.row + destRow, curTigLoc.col + destCol};
            Move_t m{game[0], dst};
            if(legalMove(game, m)){
                moves.push_back(m);
            }
        }
    }
    return moves;
}

//Supposed to detect if their is a jump available but currently doesn't work
//FIX
bool jumpMoves(const Point_t& curLoc, const Point_t& jumpLoc){
    return abs(curLoc.row-jumpLoc.row)==2 or abs(curLoc.col - jumpLoc.col) == 2;
}

Move_t tigerMoveAlgo(vector<Token_t> game){
    Point_t tigerLoc = game[0].location;
    Zones zone = getCurZone(tigerLoc);
    vector<Move_t> moves = possibleTigerMoves(game);
    
    //Need to fix jump algorithm
    vector<Move_t> jumps;
    for(auto& m : moves){
        if(jumpMoves(tigerLoc, m.destination)){
            jumps.push_back(m);
        }
    }
    
    if(zone == GREENZONE){
        if(!jumps.empty()){
            return jumps[rand()%jumps.size()];
        }
    }
    
    if(zone == YELLOWZONE){
            //Look for isolated soldier jumps
            for(auto& m : jumps){
                Point_t mid{(tigerLoc.row + m.destination.row)/2, (tigerLoc.col + m.destination.col)/2};
                int neighbors = 0;
                for(size_t i = 1; i < game.size(); i++){
                    if(game[i].location.row == mid.row  and game[i].location.col == mid.col){
                        //count nearby men
                        for(size_t j = 1; j < game.size(); j++){
                            if(i == j) continue;
                            if(dist(game[i].location, game[j].location) <= 1.5){
                                neighbors++;
                            }
                        }

                        if(neighbors == 0){
                            return m;  // take isolated soldier
                        }
                    }
                }
            }

            // Else: move closest to green zone
            Move_t best = moves[0];
            int minDist = INT_MAX;
            for(auto& m : moves){
                int d = abs(m.destination.row - 8);  // bottom = rows 8–12
                if(d < minDist){
                    minDist = d;
                    best = m;
                }
            }
            return best;
        }

        // RED ZONE: retreat to green — ignore soldiers
        if(zone == REDZONE){
            Move_t best = moves[0];
            int minDist = 1e9;
            for(auto& m : moves){
                int d = abs(m.destination.row - 8);
                if(d < minDist){
                    minDist = d;
                    best = m;
                }
            }
            return best;
        }

        // Fallback: random move
        return moves[rand() % moves.size()];
}
/*

RedZoneBehavior
YellowZoneBehavior
GreenZoneBehavior

*/
#endif /* Tiger_h */
