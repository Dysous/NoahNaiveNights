//
// Created by NTMat on 4/19/2025.
//

#include "Tiger.h"
#include <vector>
#include "global.h"
using namespace std;

class Tiger {
private:
    vector<Token_t> tokens;

public:
    const vector<Token_t>& getTokens() const {
        return tokens;
    }
};


