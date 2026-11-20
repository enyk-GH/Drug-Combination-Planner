#pragma once
#include <string>
using namespace std;
struct Drug {
    int id;
    string name;
    int benefit;
    int toxicity;
    Drug(int id, string name, int benefit, int toxicity) {
        this->id = id; this->name = name; this->benefit = benefit; this->toxicity = toxicity;
    }
};
