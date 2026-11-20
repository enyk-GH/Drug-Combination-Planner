#pragma once
#include "Drug.h"
#include "InteractionGraph.h"
#include <vector>
#include <string>
using namespace std;
enum class GreedyStrategy { RATIO, HIGHEST_BENEFIT, LOWEST_TOXICITY, INTERACTION_AWARE };
struct GreedyResult {
    vector<Drug> selected;
    vector<Drug> rejected;
    vector<string> rejectionReasons;
};
class GreedySelector {
public:
    static GreedyResult run(const vector<Drug>& drugs, const InteractionGraph& graph, GreedyStrategy strategy);
};
