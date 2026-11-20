#pragma once
#include <unordered_map>
#include <set>
#include <vector>
using namespace std;
class InteractionGraph {
private:
    unordered_map<int, set<int>> adj;
public:
    void addVertex(int drugID);
    bool addInteraction(int id1, int id2);
    void removeDrug(int drugID);
    bool hasConflict(int id1, int id2) const;
    bool isCompatible(const vector<int>& selectedIDs, int candidateID) const;
    int getDegree(int drugID) const;
    void displayGraph() const;
};
