#include "DrugManager.h"
#include <iostream>
using namespace std;
bool DrugManager::addDrug(const Drug& drug) {
    if (drug.id < 0 || drug.benefit < 0 || drug.toxicity < 0) { cout << "Invalid drug information.\n"; return false; }
    if (containsDrug(drug.id)) { cout << "Drug ID already exists.\n"; return false; }
    drugs.push_back(drug);
    cout << "Drug added successfully.\n";
    return true;
}
bool DrugManager::removeDrug(int id) {
    for (size_t i = 0; i < drugs.size(); i++) {
        if (drugs[i].id == id) { drugs.erase(drugs.begin() + i); cout << "Drug removed successfully.\n"; return true; }
    }
    cout << "Drug not found.\n";
    return false;
}
void DrugManager::displayDrugs() const {
    if (drugs.empty()) { cout << "No drugs available.\n"; return; }
    cout << "\nID\tName\tBenefit\tToxicity\n";
    for (const Drug& d : drugs) cout << d.id << "\t" << d.name << "\t" << d.benefit << "\t" << d.toxicity << "\n";
}
const vector<Drug>& DrugManager::getDrugs() const { return drugs; }
bool DrugManager::containsDrug(int id) const {
    for (const Drug& d : drugs) if (d.id == id) return true;
    return false;
}
