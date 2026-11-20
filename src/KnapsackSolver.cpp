#include "KnapsackSolver.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace {
std::string escapeXml(const std::string& text) {
    std::string out;
    out.reserve(text.size());
    for (char ch : text) {
        switch (ch) {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default:   out += ch;       break;
        }
    }
    return out;
}
}

SolveResult KnapsackSolver::solve(const std::vector<Drug>& candidates, int budget) {
    auto startTime = std::chrono::high_resolution_clock::now();
    SolveResult res;

    if (budget < 0) {
        res.feasible = false;
        res.note = "Negative toxicity budget is infeasible";
        dp.clear();
        lastRowLabels_.clear();
        auto endTime = std::chrono::high_resolution_clock::now();
        res.runtimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return res;
    }

    std::vector<Drug> filtered;
    filtered.reserve(candidates.size());
    lastRowLabels_.clear();
    for (const auto& d : candidates) {
        if (d.toxicity >= 0 && d.toxicity <= budget) {
            filtered.push_back(d);
            lastRowLabels_.push_back(d.name);
        }
    }

    int n = static_cast<int>(filtered.size());

    dp.assign(n + 1, std::vector<int>(budget + 1, 0));

    if (n == 0) {
        if (!candidates.empty()) {
            res.note = "All candidate drugs exceed budget";
        } else {
            res.note = "Empty candidate list";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        res.runtimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return res;
    }

    for (int i = 1; i <= n; ++i) {
        int ben = filtered[i - 1].benefit;
        int tox = filtered[i - 1].toxicity;

        for (int w = 0; w <= budget; ++w) {
            dp[i][w] = dp[i - 1][w];

            if (tox <= w) {
                int candidateBenefit = dp[i - 1][w - tox] + ben;
                if (candidateBenefit > dp[i][w]) {
                    dp[i][w] = candidateBenefit;
                }
            }
        }
    }

    int w = budget;
    for (int i = n; i >= 1; --i) {
        if (dp[i][w] != dp[i - 1][w]) {
            const Drug& d = filtered[i - 1];
            res.chosen.push_back(d);
            res.totalBenefit += d.benefit;
            res.totalToxicity += d.toxicity;
            w -= d.toxicity;
        }
    }

    std::reverse(res.chosen.begin(), res.chosen.end());

    auto endTime = std::chrono::high_resolution_clock::now();
    res.runtimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    return res;
}

void KnapsackSolver::printTable() const {
    if (dp.empty() || dp[0].empty()) {
        std::cout << "[KnapsackSolver] DP table is empty.\n";
        return;
    }

    int rows = static_cast<int>(dp.size());
    int cols = static_cast<int>(dp[0].size());

    std::size_t labelWidth = 10;
    for (const auto& name : lastRowLabels_) {
        if (name.size() + 2 > labelWidth) {
            labelWidth = name.size() + 2;
        }
    }

    std::cout << std::left << std::setw(static_cast<int>(labelWidth)) << "Drug \\ Tox";
    for (int w = 0; w < cols; ++w) {
        std::cout << std::right << std::setw(5) << w;
    }
    std::cout << "\n" << std::string(labelWidth + 5 * cols, '-') << "\n";

    for (int i = 0; i < rows; ++i) {
        std::string rowLabel = (i == 0) ? "(none)" : lastRowLabels_[i - 1];
        std::cout << std::left << std::setw(static_cast<int>(labelWidth)) << rowLabel;
        for (int w = 0; w < cols; ++w) {
            std::cout << std::right << std::setw(5) << dp[i][w];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

bool KnapsackSolver::saveTableAsImage(const std::string& filename) const {
    if (dp.empty() || dp[0].empty()) {
        return false;
    }

    std::ofstream out(filename);
    if (!out.is_open()) {
        return false;
    }

    int rows = static_cast<int>(dp.size());
    int cols = static_cast<int>(dp[0].size());

    std::size_t maxLabelLen = 10;
    for (const auto& name : lastRowLabels_) {
        if (name.size() > maxLabelLen) {
            maxLabelLen = name.size();
        }
    }

    const int padding = 20;
    const int titleH = 42;
    const int labelColW = std::max(140, static_cast<int>(maxLabelLen * 9 + 28));
    const int cellW = 54;
    const int cellH = 34;

    const int tableX = padding;
    const int tableY = padding + titleH;
    const int totalW = padding * 2 + labelColW + cols * cellW;
    const int totalH = padding * 2 + titleH + (rows + 1) * cellH;

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << totalW
        << "\" height=\"" << totalH << "\" viewBox=\"0 0 " << totalW << " " << totalH << "\">\n"
        << "  <rect width=\"100%\" height=\"100%\" fill=\"#ffffff\"/>\n"
        << "  <text x=\"" << tableX << "\" y=\"" << (padding + 26)
        << "\" font-family=\"Consolas, Arial, sans-serif\" font-size=\"18\" font-weight=\"bold\" fill=\"#0f172a\">"
        << "0/1 Knapsack DP Table (Budget = " << (cols - 1) << ")</text>\n";

    out << "  <rect x=\"" << tableX << "\" y=\"" << tableY << "\" width=\"" << labelColW
        << "\" height=\"" << cellH << "\" fill=\"#1e293b\" stroke=\"#cbd5e1\"/>\n"
        << "  <text x=\"" << (tableX + 12) << "\" y=\"" << (tableY + 22)
        << "\" font-family=\"Consolas, Arial, sans-serif\" font-size=\"13\" font-weight=\"bold\" fill=\"#ffffff\">"
        << "Drug \\ Tox</text>\n";

    for (int w = 0; w < cols; ++w) {
        int x = tableX + labelColW + w * cellW;
        out << "  <rect x=\"" << x << "\" y=\"" << tableY << "\" width=\"" << cellW
            << "\" height=\"" << cellH << "\" fill=\"#1e293b\" stroke=\"#cbd5e1\"/>\n"
            << "  <text x=\"" << (x + cellW / 2) << "\" y=\"" << (tableY + 22)
            << "\" text-anchor=\"middle\" font-family=\"Consolas, Arial, sans-serif\" font-size=\"13\" font-weight=\"bold\" fill=\"#ffffff\">"
            << w << "</text>\n";
    }

    for (int i = 0; i < rows; ++i) {
        int y = tableY + (i + 1) * cellH;
        std::string rowFill = (i % 2 == 0) ? "#f8fafc" : "#ffffff";
        std::string rowLabel = (i == 0) ? "(none)" : escapeXml(lastRowLabels_[i - 1]);

        out << "  <rect x=\"" << tableX << "\" y=\"" << y << "\" width=\"" << labelColW
            << "\" height=\"" << cellH << "\" fill=\"" << rowFill << "\" stroke=\"#cbd5e1\"/>\n"
            << "  <text x=\"" << (tableX + 12) << "\" y=\"" << (y + 22)
            << "\" font-family=\"Consolas, Arial, sans-serif\" font-size=\"13\" font-weight=\"bold\" fill=\"#1e293b\">"
            << rowLabel << "</text>\n";

        for (int w = 0; w < cols; ++w) {
            int x = tableX + labelColW + w * cellW;
            bool isOptimalCell = (i == rows - 1 && w == cols - 1);
            std::string cellFill = isOptimalCell ? "#dcfce7" : rowFill;
            std::string textFill = isOptimalCell ? "#166534" : "#0f172a";
            std::string weight = isOptimalCell ? "bold" : "normal";

            out << "  <rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << cellW
                << "\" height=\"" << cellH << "\" fill=\"" << cellFill << "\" stroke=\"#cbd5e1\"/>\n"
                << "  <text x=\"" << (x + cellW / 2) << "\" y=\"" << (y + 22)
                << "\" text-anchor=\"middle\" font-family=\"Consolas, Arial, sans-serif\" font-size=\"13\" font-weight=\""
                << weight << "\" fill=\"" << textFill << "\">" << dp[i][w] << "</text>\n";
        }
    }

    out << "</svg>\n";
    return out.good();
}