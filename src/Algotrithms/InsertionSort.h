

#ifndef INSERTIONSORT_H
#define INSERTIONSORT_H
#include <vector>

#include "SortingAlgorithm.h"


class InsertionSort: public SortAlgorithm{

public:
    std::vector<int> sort(std::vector<int> arr);

private:
    void insertionSort(std::vector<int> arr);


};



#endif //INSERTIONSORT_H
