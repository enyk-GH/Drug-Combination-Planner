#include "DrugManager.h"
#include "InteractionGraph.h"
#include "GreedySelector.h"
#include "KnapsackSolver.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

static string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) { cout << "\n"; exit(0); }
    return line;
}

static int readInt(const string& prompt) {
    while (true) {
        string line = readLine(prompt);
        try {
            size_t pos = 0;
            int v = stoi(line, &pos);
            if (pos == line.size()) return v;
        } catch (...) {}
        cout << "  Please enter a whole number.\n";
    }
}

static void pause() { readLine("\nPress Enter to continue..."); }

struct App {
    DrugManager manager;
    InteractionGraph graph;
    int budget = 8;

    bool addDrug(int id, const string& name, int ben, int tox) {
        if (manager.addDrug(Drug(id, name, ben, tox))) { graph.addVertex(id); return true; }
        return false;
    }
};

static void actionAddDrug(App& app) {
    int id = readInt("Drug ID (number): ");
    string name = readLine("Name: ");
    int ben = readInt("Benefit (whole number): ");
    int tox = readInt("Toxicity (whole number): ");
    app.addDrug(id, name, ben, tox);
}

static void actionRemoveDrug(App& app) {
    int id = readInt("ID of drug to remove: ");
    if (app.manager.removeDrug(id)) app.graph.removeDrug(id);
}

static void actionAddInteraction(App& app) {
    int a = readInt("First drug ID: ");
    int b = readInt("Second drug ID: ");
    app.graph.addInteraction(a, b);
}

static void actionSetBudget(App& app) {
    int b = readInt("New toxicity budget: ");
    if (b < 0) cout << "Budget cannot be negative.\n"; else app.budget = b;
}

static GreedyStrategy pickStrategy() {
    cout << "\n1. Highest benefit/toxicity ratio\n2. Highest benefit\n"
            "3. Lowest toxicity\n4. Interaction-aware\n";
    while (true) {
        int c = readInt("Strategy (1-4): ");
        switch (c) {
            case 1: return GreedyStrategy::RATIO;
            case 2: return GreedyStrategy::HIGHEST_BENEFIT;
            case 3: return GreedyStrategy::LOWEST_TOXICITY;
            case 4: return GreedyStrategy::INTERACTION_AWARE;
        }
        cout << "  Choose 1, 2, 3 or 4.\n";
    }
}

static void actionGreedyDetail(App& app) {
    if (app.manager.getDrugs().empty()) { cout << "Add some drugs first.\n"; return; }
    GreedyStrategy s = pickStrategy();

    vector<Drug> usable;
    for (const Drug& d : app.manager.getDrugs())
        if (d.toxicity <= app.budget) usable.push_back(d);

    GreedyResult g = GreedySelector::run(usable, app.graph, s);

    cout << "\nGreedy accepted (in selection order):\n";
    for (const Drug& d : g.selected)
        cout << "  " << d.name << " (id " << d.id << ", benefit " << d.benefit
             << ", toxicity " << d.toxicity << ")\n";
    if (g.selected.empty()) cout << "  (none)\n";

    cout << "\nGreedy rejected:\n";
    for (const string& r : g.rejectionReasons) cout << "  " << r << "\n";
    if (g.rejected.empty()) cout << "  (none)\n";

    KnapsackSolver ks;
    SolveResult r = ks.solve(g.selected, app.budget);
    cout << "\nKnapsack DP table over the accepted drugs (budget " << app.budget << "):\n";
    ks.printTable();

    const string imageFile = "dp_table.svg";
    if (ks.saveTableAsImage(imageFile)) {
        cout << "DP table image saved to: " << imageFile << "\n";
    } else {
        cout << "Could not save DP table image.\n";
    }

    cout << "DP picks: ";
    for (const Drug& d : r.chosen) cout << d.name << " ";
    if (r.chosen.empty()) cout << "(none)";
    cout << "\nBenefit " << r.totalBenefit << ", toxicity " << r.totalToxicity << "\n";
}

static void printMenu(const App& app) {
    cout << "\n=========================================\n"
            "        DRUG COMBINATION PLANNER\n"
            "=========================================\n"
            " Drugs: " << app.manager.getDrugs().size()
         << "   Toxicity budget: " << app.budget << "\n"
            "-----------------------------------------\n"
            " 1. Add drug\n"
            " 2. Remove drug\n"
            " 3. Show all drugs\n"
            " 4. Add interaction (conflict)\n"
            " 5. Show interaction graph\n"
            " 6. Set toxicity budget\n"
            " 7. Greedy + DP in detail (one strategy)\n"
            " 0. Exit\n"
            "-----------------------------------------\n";
}

int main() {
    App app;
    cout << "Educational model with simplified data. Not clinical advice.\n";

    while (true) {
        printMenu(app);
        int choice = readInt("Choose an option: ");
        cout << "\n";
        switch (choice) {
            case 1:  actionAddDrug(app); break;
            case 2:  actionRemoveDrug(app); break;
            case 3:  app.manager.displayDrugs(); break;
            case 4:  actionAddInteraction(app); break;
            case 5:  app.graph.displayGraph(); break;
            case 6:  actionSetBudget(app); break;
            case 7:  actionGreedyDetail(app); break;
            case 0:  cout << "Goodbye.\n"; readLine("Press Enter to close..."); return 0;
            default: cout << "Invalid option.\n";
        }
        pause();
    }
}