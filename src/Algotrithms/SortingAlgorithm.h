
#ifndef SORTINGALGORITHM_H
#define SORTINGALGORITHM_H
#include <iostream>
#include <ostream>

class SortAlgorithm {
protected:
    int swapCount = 0;
    int compareCount = 0;
    double runtime = 0.0;

public:
    virtual std::vector<int> sort(std::vector<int>& array) = 0;

    virtual void printStats() {
        std::cout << "swap count: " << swapCount << std::endl;
        std::cout << "compare count: " << compareCount << std::endl;
        std::cout << "runtime: " << runtime << std::endl;
    }


    virtual ~SortAlgorithm() {}
};

#endif //SORTINGALGORITHM_H
