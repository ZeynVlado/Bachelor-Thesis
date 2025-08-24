

#ifndef QUICKSORT_H
#define QUICKSORT_H
#include <vector>

#include "SortingAlgorithm.h"


class QuickSort: public SortAlgorithm {

public:
    std::vector<int> sort(std::vector<int>& array);

private:
    void quickSort(std::vector<int>& array, int low, int high);
    int partition(std::vector<int>& array, int low, int high);

};





#endif //QUICKSORT_H
