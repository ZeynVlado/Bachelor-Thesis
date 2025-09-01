

#include "DataGenerator.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

DataGenerator::DataGenerator()
    : rng(std::random_device{}())
{}

void DataGenerator::generate_Data(ArrayType type, int size, int minValue, int maxValue, int runs, int k) {
    switch (type) {
        case ArrayType::PERMUTATION_ARRAY: {
            input = generatePermutation(size);
            break;
        }
        case ArrayType::RANDOM_ARRAY: {
            input = generateRandom(size, minValue, maxValue, k);
            break;
        }
        case ArrayType::SORTED_ARRAY: {
            input = generateSorted(size, minValue, maxValue);
            break;
        }
        case ArrayType::REVERSE_ARRAY: {
            input = generateReverseSorted(size, minValue, maxValue);
            break;
        }
        case ArrayType::RUNS_ARRAY: {
            input = generateRuns(size, runs, minValue, maxValue);
            break;
        }
        default: {
            break;
        }
    }
}

void DataGenerator::printArray(const std::vector<int>& arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i != arr.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}


std::vector<int> DataGenerator::generatePermutation(size_t size) {
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 1);

    std::shuffle(data.begin(), data.end(), rng);

    return data;
}

std::vector<int> DataGenerator::generateRandom(size_t size, int minValue, int maxValue, int k) {

    int range = maxValue - minValue + 1;

    std::vector<int> pool(range);
    std::iota(pool.begin(), pool.end(), minValue);

    std::shuffle(pool.begin(), pool.end(), rng);
    pool.resize(k);

    std::vector<int> data(size);
    std::uniform_int_distribution<int> dist(0, k - 1);

    for (auto& val : data) {
        val = pool[dist(rng)];
    }

    return data;
}

std::vector<int> DataGenerator::generateSorted(size_t size, int minValue, int maxValue) {
    std::vector<int> data(size);
    if (size == 0) return data;

    std::uniform_int_distribution<int> firstDist(minValue, static_cast<int>(0.3 * maxValue));
    data[0] = firstDist(rng);

    for (size_t i = 1; i < size; ++i) {
        int maxStep = (maxValue - data[i - 1]) / static_cast<int>(size - i);
        maxStep = std::max(maxStep, 1);
        std::uniform_int_distribution<int> stepDist(1, maxStep);
        data[i] = data[i - 1] + stepDist(rng);
    }

    return data;
}

std::vector<int> DataGenerator::generateReverseSorted(size_t size, int minValue, int maxValue) {
    std::vector<int> data(size);
    if (size == 0) return data;

    std::uniform_int_distribution<int> firstDist(static_cast<int>(0.7 * minValue), maxValue);
    data[0] = firstDist(rng);

    for (size_t i = 1; i < size; ++i) {
        int maxStep = (data[i - 1] - minValue) / static_cast<int>(size - i);
        maxStep = std::max(maxStep, 1);
        std::uniform_int_distribution<int> stepDist(1, maxStep);
        data[i] = data[i - 1] - stepDist(rng);
    }

    return data;
}



std::vector<int> DataGenerator::generateRuns(size_t size, size_t runsCount, int minValue, int maxValue) {
    std::vector<int> data;
    if (runsCount == 0 || size == 0) return data;

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> distLength(
        1,
        static_cast<int>(std::max<size_t>(1, size / std::max<size_t>(runsCount, 1)))
    );


    const auto runLengths = buildRunLengths(size, runsCount, gen, distLength);
    const auto runStarts  = buildRunStarts(runLengths);

    for (size_t i = 0; i < runsCount; ++i) {
        const size_t L = runLengths[i];
        if (L == 0) continue;

        if (!ensurePrevRunFeasible(gen, i, L, runLengths, runStarts, minValue, maxValue, data)) {
            throw std::runtime_error("Failed to regenerate previous runs to satisfy constraints");
        }

        const bool hasPrev = !data.empty();
        const int prevLast = hasPrev ? data.back() : std::numeric_limits<int>::max();

        std::vector<int> runElems;
        if (!generateRunWithRetries(gen, L, minValue, maxValue, hasPrev, prevLast, runElems, /*attempts*/100)) {
            throw std::runtime_error("Unable to generate run with given constraints");
        }

        data.insert(data.end(), runElems.begin(), runElems.end());
    }

    return data;
}


bool DataGenerator::generateSingleRun(std::mt19937& gen,
                                      size_t L,
                                      int minValue,
                                      int maxValue,
                                      bool hasPrev,
                                      int prevLast,
                                      std::vector<int>& out)
{
    if (L == 0) return true;

    long long hi = static_cast<long long>(maxValue) - static_cast<long long>(L) + 1LL;

    if (hasPrev) {
        hi = std::min<long long>(hi, static_cast<long long>(prevLast) - 1LL);
    }

    long long lo = minValue;
    if (L == 1) {

        lo = std::max<long long>(lo, static_cast<long long>(minValue) + 1LL);
    }

    if (hi < lo) {
        return false;
    }

    std::uniform_int_distribution<int> distStart(static_cast<int>(lo), static_cast<int>(hi));
    int startValue = distStart(gen);

    int current = startValue;
    int remainingLength = static_cast<int>(L);
    int remainingRange  = std::max(0, maxValue - current);

    for (size_t j = 0; j < L; ++j) {
        out.push_back(current);
        --remainingLength;

        if (remainingLength > 0) {

            int maxPossibleStep = std::max(1, remainingRange / remainingLength);
            std::uniform_int_distribution<int> distStep(1, maxPossibleStep);
            int step = distStep(gen);

            long long nextVal = static_cast<long long>(current) + static_cast<long long>(step);
            if (nextVal > maxValue) nextVal = maxValue;

            current = static_cast<int>(nextVal);
            remainingRange = std::max(0, maxValue - current);
        }
    }

    return true;
}

bool DataGenerator::regeneratePreviousRun(std::mt19937& gen,
                                          size_t k,
                                          const std::vector<size_t>& runLengths,
                                          const std::vector<size_t>& runStarts,
                                          int minValue,
                                          int maxValue,
                                          std::vector<int>& data)
{
    const size_t L        = runLengths[k];
    const size_t startIdx = runStarts[k];

    data.erase(data.begin() + static_cast<long long>(startIdx),
               data.begin() + static_cast<long long>(startIdx) + static_cast<long long>(L));


    const bool hasPrev2 = (k > 0);
    int prevLast2 = 0;
    if (hasPrev2) {
        const size_t endIdxPrev2 = startIdx - 1;
        prevLast2 = data[endIdxPrev2];
    }


    for (int attempt = 0; attempt < 100; ++attempt) {
        std::vector<int> candidate;
        const bool ok = generateSingleRun(gen, L, minValue, maxValue, hasPrev2, prevLast2, candidate);
        if (!ok) continue;

        if (L == 1 && candidate[0] == minValue) continue;

        data.insert(data.begin() + static_cast<long long>(startIdx), candidate.begin(), candidate.end());
        return true;
    }

    return false;
}

std::vector<size_t> DataGenerator::buildRunLengths(size_t size,
                                                   size_t runsCount,
                                                   std::mt19937& gen,
                                                   std::uniform_int_distribution<int>& distLength)
{
    std::vector<size_t> runLengths(runsCount, 0);
    size_t total = 0;

    for (size_t i = 0; i < runsCount && total < size; ++i) {
        size_t len = static_cast<size_t>(distLength(gen));
        if (total + len > size) len = size - total;
        runLengths[i] = len;
        total += len;
    }

    size_t remaining = size - total;
    while (remaining--) {
        size_t idx = gen() % std::max<size_t>(runsCount, 1);
        ++runLengths[idx];
    }

    return runLengths;
}

std::vector<size_t> DataGenerator::buildRunStarts(const std::vector<size_t>& runLengths) {
    std::vector<size_t> runStarts(runLengths.size(), 0);
    size_t cursor = 0;
    for (size_t i = 0; i < runLengths.size(); ++i) {
        runStarts[i] = cursor;
        cursor += runLengths[i];
    }
    return runStarts;
}

bool DataGenerator::needBacktrackForNextRun(size_t L,
                                            int prevLast,
                                            int minValue,
                                            int maxValue)
{
    long long hi = static_cast<long long>(maxValue) - static_cast<long long>(L) + 1LL;
    hi = std::min<long long>(hi, static_cast<long long>(prevLast) - 1LL);
    long long lo = minValue;
    return (hi < lo);
}

bool DataGenerator::generateRunWithRetries(std::mt19937& gen,
                                           size_t L,
                                           int minValue,
                                           int maxValue,
                                           bool hasPrev,
                                           int prevLast,
                                           std::vector<int>& out,
                                           int attempts)
{
    for (int a = 0; a < attempts; ++a) {
        out.clear();
        if (generateSingleRun(gen, L, minValue, maxValue, hasPrev, prevLast, out)) {
            return true;
        }
    }
    return false;
}

bool DataGenerator::ensurePrevRunFeasible(std::mt19937& gen,
                                          size_t currentRunIndex,
                                          size_t L,
                                          const std::vector<size_t>& runLengths,
                                          const std::vector<size_t>& runStarts,
                                          int minValue,
                                          int maxValue,
                                          std::vector<int>& data)
{
    if (data.empty()) return true;

    size_t k = currentRunIndex - 1;
    while (true) {
        const int prevLast = data.back();
        if (!needBacktrackForNextRun(L, prevLast, minValue, maxValue)) {
            return true;
        }

        if (!regeneratePreviousRun(gen, k, runLengths, runStarts, minValue, maxValue, data)) {
            if (k == 0) return false;
            --k;
        }

    }
}








