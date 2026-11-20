#pragma once
#include "Drug.h"
#include <vector>
using namespace std;
class DrugManager {
private:
    vector<Drug> drugs;
public:
    bool addDrug(const Drug& drug);
    bool removeDrug(int id);
    void displayDrugs() const;
    const vector<Drug>& getDrugs() const;
    bool containsDrug(int id) const;
};
