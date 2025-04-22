//
// Created by NTMat on 4/19/2025.
//

#include "Soldiers.h"
#include <vector>
#include "global.h"
using namespace std;



class Soldier {
private:
    vector<Token_t> tokens;

public:
    const vector<Token_t>& getTokens() const {
        return tokens;
    }
};