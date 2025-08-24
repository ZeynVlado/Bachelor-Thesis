

#ifndef ADAPTIVESHIVERSSORT_H
#define ADAPTIVESHIVERSSORT_H
#include <cmath>
#include <functional>
#include <stack>

#include "SortingAlgorithm.h"


struct Run;

class AdaptiveShiversSort: public SortAlgorithm {
public:
    // Статистика
    long long totalMergeCosts = 0;
    long long totalBufferCosts = 0;
    long long swapCount = 0;

    std::vector<int> sort(std::vector<int> &array);

private:

    void mergeRuns(std::vector<int>& A, Run& R1, Run& R2);

std::vector<Run> runDecomposition(const std::vector<int>& A);

void adaptiveShiversSort(std::vector<int>& A, int c);


};
#endif //ADAPTIVESHIVERSSORT_H
