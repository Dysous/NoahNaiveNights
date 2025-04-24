//
// Created by NTMat on 4/19/2025.
//

#include "Tiger.h"
#include <vector>
using namespace std;

class Tiger {
private:
    vector<Token_t> tokens;

public:
    const vector<Token_t>& getTokens() const {
        return tokens;
    }
};


