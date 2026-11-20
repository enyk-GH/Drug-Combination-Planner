#include "InteractionGraph.h"
#include <iostream>
using namespace std;
void InteractionGraph::addVertex(int drugID) { adj[drugID]; }
bool InteractionGraph::addInteraction(int id1, int id2) {
    if (id1 == id2) { cout << "A drug cannot conflict with itself.\n"; return false; }
    if (adj.find(id1) == adj.end() || adj.find(id2) == adj.end()) { cout << "One or both drug IDs do not exist in graph.\n"; return false; }
    if (hasConflict(id1, id2)) { cout << "Interaction already exists.\n"; return false; }
    adj[id1].insert(id2); adj[id2].insert(id1);
    cout << "Interaction added successfully.\n";
    return true;
}
void InteractionGraph::removeDrug(int drugID) {
    auto it = adj.find(drugID);
    if (it == adj.end()) return;
    for (int nb : it->second) adj[nb].erase(drugID);
    adj.erase(drugID);
}
bool InteractionGraph::hasConflict(int id1, int id2) const {
    auto it = adj.find(id1);
    if (it == adj.end()) return false;
    return it->second.find(id2) != it->second.end();
}
bool InteractionGraph::isCompatible(const vector<int>& sel, int c) const {
    for (int s : sel) if (hasConflict(s, c)) return false;
    return true;
}
int InteractionGraph::getDegree(int drugID) const {
    auto it = adj.find(drugID);
    if (it == adj.end()) return 0;
    return static_cast<int>(it->second.size());
}
void InteractionGraph::displayGraph() const {
    cout << "\n===== INTERACTION GRAPH =====\n";
    for (const auto& e : adj) {
        cout << "Drug " << e.first << " -> ";
        if (e.second.empty()) cout << "No conflicts";
        else for (int nb : e.second) cout << nb << " ";
        cout << "\n";
    }
}
