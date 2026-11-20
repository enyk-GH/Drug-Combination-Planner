#ifndef KNAPSACK_SOLVER_H
#define KNAPSACK_SOLVER_H

#include <string>
#include <vector>
#include "Drug.h"
#include "SolveResult.h"

class KnapsackSolver {
public:
    SolveResult solve(const std::vector<Drug>& candidates, int budget);
    void printTable() const;
    bool saveTableAsImage(const std::string& filename) const;

private:
    std::vector<std::vector<int>> dp;
    std::vector<std::string> lastRowLabels_;
};

#endif