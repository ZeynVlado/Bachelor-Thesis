

#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H
#include <algorithm>
#include <random>
#include <vector>

#include "ArrayType.h"


class DataGenerator {

public:
    DataGenerator();


    std::vector<int> input;

    void generate_Data(ArrayType type, int size, int minValue, int maxValue, int runs, int k);

    void printArray(const std::vector<int>& arr);

    std::vector<int> generatePermutation(size_t size);

    std::vector<int> generateRandom(size_t size, int minValue, int maxValue, int k);
    std::vector<int> generateSorted(size_t size, int minValue, int maxValue);
    std::vector<int> generateReverseSorted(size_t size, int minValue, int maxValue);
    std::vector<int> generateRuns(size_t size, size_t runsCount, int minValue, int maxValue);
    std::vector<int> generateInversions(size_t size, size_t invCount, int minValue, int maxValue);
    std::vector<int> generateRem(size_t size, size_t remCount, int minValue, int maxValue, int k);
    std::vector<int> generateHam(size_t size, size_t hamCount, int minValue, int maxValue);
    std::vector<int> generateDis(size_t size, size_t maxDistance, int minValue, int maxValue);
    std::vector<int> generateExs(size_t size, size_t exchanges, int minValue, int maxValue);
    std::vector<int> generateOsc(size_t size, size_t oscValue, int minValue, int maxValue);

    std::vector<int> generateMax(size_t size, size_t maxDistance, int minValue, int maxValue);

    std::vector<int> getInput() {
        return input;
    };




private:

     // Generate one ascending run of length L and append it to `out`.
    // If hasPrev == true, the run's start must be < prevLast.
    // Values must stay within [minValue, maxValue].
    // For L == 1, the single value must be > minValue.
    bool generateSingleRun(std::mt19937& gen,
                           size_t L,
                           int minValue,
                           int maxValue,
                           bool hasPrev,
                           int prevLast,
                           std::vector<int>& out);

    // Regenerate the previous run k in-place within `data` using the same length.
    // Keeps it strictly below the run before it (if exists), and avoids L==1 at minValue.
    bool regeneratePreviousRun(std::mt19937& gen,
                               size_t k,
                               const std::vector<size_t>& runLengths,
                               const std::vector<size_t>& runStarts,
                               int minValue,
                               int maxValue,
                               std::vector<int>& data);

    // Build random run lengths that sum exactly to `size`.
    std::vector<size_t> buildRunLengths(size_t size,
                                        size_t runsCount,
                                        std::mt19937& gen,
                                        std::uniform_int_distribution<int>& distLength);

    // Prefix sums of lengths → start indices of each run in the final array.
    std::vector<size_t> buildRunStarts(const std::vector<size_t>& runLengths);

    // Check whether the next run of length L would have an empty start interval given prevLast.
    static bool needBacktrackForNextRun(size_t L,
                                        int prevLast,
                                        int minValue,
                                        int maxValue);

    // Retry wrapper around generateSingleRun to avoid rare random dead-ends.
    bool generateRunWithRetries(std::mt19937& gen,
                                size_t L,
                                int minValue,
                                int maxValue,
                                bool hasPrev,
                                int prevLast,
                                std::vector<int>& out,
                                int attempts = 100);

    // Ensure the previous run(s) allow starting the current run; backtracks/regenerates if needed.
    bool ensurePrevRunFeasible(std::mt19937& gen,
                               size_t currentRunIndex,
                               size_t L,
                               const std::vector<size_t>& runLengths,
                               const std::vector<size_t>& runStarts,
                               int minValue,
                               int maxValue,
                               std::vector<int>& data);


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
