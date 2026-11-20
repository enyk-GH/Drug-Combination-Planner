#include "GreedySelector.h"
#include <algorithm>
#include <iostream>
#include <limits>
using namespace std;
static double calculateScore(const Drug& drug, const InteractionGraph& graph, GreedyStrategy strategy) {
    switch (strategy) {
        case GreedyStrategy::RATIO:
            if (drug.toxicity == 0) return drug.benefit > 0 ? numeric_limits<double>::infinity() : 0.0;
            return static_cast<double>(drug.benefit) / drug.toxicity;
        case GreedyStrategy::HIGHEST_BENEFIT: return drug.benefit;
        case GreedyStrategy::LOWEST_TOXICITY: return -drug.toxicity;
        case GreedyStrategy::INTERACTION_AWARE: {
            int degree = graph.getDegree(drug.id);
            return static_cast<double>(drug.benefit) / ((drug.toxicity + 1.0) * (degree + 1.0));
        }
    }
    return 0.0;
}
GreedyResult GreedySelector::run(const vector<Drug>& drugs, const InteractionGraph& graph, GreedyStrategy strategy) {
    GreedyResult result;
    vector<Drug> orderedDrugs = drugs;
    stable_sort(orderedDrugs.begin(), orderedDrugs.end(), [&](const Drug& a, const Drug& b) {
        double sa = calculateScore(a, graph, strategy), sb = calculateScore(b, graph, strategy);
        if (sa != sb) return sa > sb;
        if (a.benefit != b.benefit) return a.benefit > b.benefit;
        if (a.toxicity != b.toxicity) return a.toxicity < b.toxicity;
        return a.id < b.id;
    });
    vector<int> selectedIDs;
    for (const Drug& c : orderedDrugs) {
        if (graph.isCompatible(selectedIDs, c.id)) { result.selected.push_back(c); selectedIDs.push_back(c.id); }
        else {
            result.rejected.push_back(c);
            for (int s : selectedIDs) if (graph.hasConflict(s, c.id)) {
                result.rejectionReasons.push_back("Drug " + to_string(c.id) + " conflicts with selected Drug " + to_string(s));
                break;
            }
        }
    }
    return result;
}
