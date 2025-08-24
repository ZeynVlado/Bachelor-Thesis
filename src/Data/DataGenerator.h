

#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H
#include <algorithm>
#include <random>
#include <vector>


class DataGenerator {

public:
    DataGenerator();

    void printArray(const std::vector<int>& arr);

    std::vector<int> generateRandom(size_t size, int minValue, int maxValue);
    std::vector<int> generateSorted(size_t size, int minValue, int maxValue);
    std::vector<int> generateReverseSorted(size_t size, int minValue, int maxValue);
    std::vector<int> generateRuns(size_t size, size_t runsCount, int minValue, int maxValue);
    std::vector<int> generateInversions(size_t size, size_t invCount, int minValue, int maxValue);
    std::vector<int> generateRem(size_t size, size_t remCount, int minValue, int maxValue);
    std::vector<int> generateHam(size_t size, size_t hamCount, int minValue, int maxValue);
    std::vector<int> generateDis(size_t size, size_t maxDistance, int minValue, int maxValue);
    std::vector<int> generateExs(size_t size, size_t exchanges, int minValue, int maxValue);
    std::vector<int> generateOsc(size_t size, size_t oscValue, int minValue, int maxValue);

    std::vector<int> generateMax(size_t size, size_t maxDistance, int minValue, int maxValue);




private:
    std::vector<int> generateRunLengths(size_t size, size_t runsCount);

    std::vector<int> generateRandomElements(size_t count, int minValue, int maxValue);
    std::vector<int> mergeLISandRemovals(const std::vector<int>& lis,
                                         const std::vector<int>& removals);

    std::vector<size_t> generateDisplacementMapping(size_t size, size_t maxDistance);
    void applyMapping(std::vector<int>& arr, const std::vector<size_t>& mapping);

    std::vector<int> createBaseArray(size_t size, int minValue, int maxValue);

    std::vector<std::vector<size_t>> splitIntoCycles(
        std::vector<size_t>& indices, size_t cyclesCount
    );

    void rotateCycles(std::vector<std::vector<size_t>>& cycles);

    std::vector<int> applyPermutation(
        const std::vector<int>& base,
        const std::vector<std::vector<size_t>>& cycles,
        const std::vector<size_t>& indices
    );

    void applyOscillation(std::vector<int>& arr, size_t oscValue);



private:
    std::mt19937 rng;
};



#endif //DATAGENERATOR_H
