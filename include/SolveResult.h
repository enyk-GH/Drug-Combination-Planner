#ifndef SOLVE_RESULT_H
#define SOLVE_RESULT_H

#include <string>
#include <vector>
#include "Drug.h"

struct SolveResult {
    std::vector<Drug> chosen;
    int totalBenefit = 0;
    int totalToxicity = 0;
    double runtimeMs = 0.0;
    bool feasible = true;   
    std::string note;       
};

#endif