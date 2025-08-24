

#ifndef MERGESORT_H
#define MERGESORT_H
#include <vector>

#include "SortingAlgorithm.h"


class MergeSort: public SortAlgorithm{

public:

    std::vector<int> sort(std::vector<int>& array);

private:

    bool COUNT_MERGE_COSTS = true;
    long long totalMergeCosts = 0;
    long long totalBufferCosts = 0;

    void merge(std::vector<int>& arr, int left, int mid, int right);

    void merge_runs_copy_half(std::vector<int>& A, int l, int m, int r, std::vector<int>& B);

    void merge_runs_basic(std::vector<int>& A, int l, int m, int r, std::vector<int>& B);

    void merge_runs_basic_sentinels(std::vector<int>& A, int l, int m, int r, std::vector<int>& B);

    void mergeSort(std::vector<int>& arr, int left, int right);

};



#endif //MERGESORT_H
