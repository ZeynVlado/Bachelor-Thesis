
#include "AdaptiveShiversSort.h"

#include <chrono>


struct Run {
    int start;
    int end;
    int length() const { return end - start; }
};

std::vector<int> AdaptiveShiversSort::sort(std::vector<int>& array) {
    std::vector<int> arr = array;
    swapCount = 0;

    auto start = std::chrono::high_resolution_clock::now();

    adaptiveShiversSort(arr, 3);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;

    std::cout << "Swaps: " << swapCount << std::endl;
    std::cout << "Runtime: " << runtime.count() << " seconds" << std::endl;

    return arr;
}


void AdaptiveShiversSort:: mergeRuns(std::vector<int>& A, Run& R1, Run& R2) {
    std::vector<int> temp;
    int i = R1.start, j = R2.start;
    while (i <= R1.end && j <= R2.end) {
        if (A[i] <= A[j]) temp.push_back(A[i++]);
        else temp.push_back(A[j++]);
    }
    while (i <= R1.end) temp.push_back(A[i++]);
    while (j <= R2.end) temp.push_back(A[j++]);

    for (int k = 0; k < temp.size(); ++k) {
        A[R1.start + k] = temp[k];
    }

    R1.end = R2.end;
}

std::vector<Run> AdaptiveShiversSort::  runDecomposition(const std::vector<int>& A) {
    std::vector<Run> runs;
    int n = A.size();
    if (n == 0) return runs;

    int start = 0;
    for (int i = 1; i < n; ++i) {
        if (A[i] < A[i-1]) {
            runs.push_back({start, i-1});
            start = i;
        }
    }
    runs.push_back({start, n-1});
    return runs;
}

void AdaptiveShiversSort:: adaptiveShiversSort(std::vector<int>& A, int c) {
    std::vector<Run> runs = runDecomposition(A);
    std::vector<Run> S;

    auto l = [&](const Run& r) -> int {
        return (int)std::floor(std::log2(r.length() / (double)c));
    };

    while (true) {
        int h = S.size();

        auto top = [&](int i) -> Run& {
            return S[S.size() - i];
        };

        if (h >= 3 && l(top(1)) >= l(top(3))) {
            Run R2 = top(2);
            Run R3 = top(3);
            mergeRuns(A, R2, R3);
            S.erase(S.end() - 3, S.end() - 1);
            S.push_back(R2);
        } else if (h >= 3 && l(top(2)) >= l(top(3))) {
            Run R2 = top(2);
            Run R3 = top(3);
            mergeRuns(A, R2, R3);
            S.erase(S.end() - 3, S.end() - 1);
            S.push_back(R2);
        } else if (h >= 2 && l(top(1)) >= l(top(2))) {
            Run R1 = top(1);
            Run R2 = top(2);
            mergeRuns(A, R1, R2);
            S.erase(S.end() - 2, S.end());
            S.push_back(R1);
        } else if (!runs.empty()) {
            S.push_back(runs.back());
            runs.pop_back();
        } else {
            break;
        }
    }


    while (S.size() > 1) {
        Run R1 = S[S.size() - 2];
        Run R2 = S[S.size() - 1];
        mergeRuns(A, R1, R2);
        S.pop_back();
        S.pop_back();
        S.push_back(R1);
    }
}