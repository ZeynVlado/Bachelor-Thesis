

#ifndef BINARYINSERTIONSORT_H
#define BINARYINSERTIONSORT_H
#include <vector>

#include "SortingAlgorithm.h"


class BinaryInsertionSort: public SortAlgorithm {

public:
    std::vector<int> sort(std::vector<int>& array);


private:

    int binarySearch(const std::vector<int>& arr, int key, int low, int high);

    void binaryInsertionSort(std::vector<int>& arr);
};



#endif //BINARYINSERTIONSORT_H
