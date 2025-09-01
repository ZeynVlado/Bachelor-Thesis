

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

    // 2) Generate runs with backtracking when needed
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

//

bool DataGenerator::generateSingleRun(std::mt19937& gen,
                                      size_t L,
                                      int minValue,
                                      int maxValue,
                                      bool hasPrev,
                                      int prevLast,
                                      std::vector<int>& out)
{
    if (L == 0) return true;

    // Upper bound for the start value to ensure enough space for L-1 increments
    long long hi = static_cast<long long>(maxValue) - static_cast<long long>(L) + 1LL;

    if (hasPrev) {
        hi = std::min<long long>(hi, static_cast<long long>(prevLast) - 1LL);
    }

    long long lo = minValue;
    if (L == 1) {

        lo = std::max<long long>(lo, static_cast<long long>(minValue) + 1LL);
    }

    if (hi < lo) {
        return false; // Constraints cannot be satisfied
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

    // Remove the old run k
    data.erase(data.begin() + static_cast<long long>(startIdx),
               data.begin() + static_cast<long long>(startIdx) + static_cast<long long>(L));

    // Determine the last element of the run before k
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


std::vector<int> DataGenerator::generateInversions(size_t size, size_t invCount,
                                                   int minValue, int maxValue) {


    std::vector<int> arr = generateSorted(size, minValue, maxValue);


    std::mt19937 rng(std::random_device{}());
    std::vector<int> result;
    result.reserve(size);

    size_t remainingInv = invCount;

    for (size_t i = 0; i < size; ++i) {

        size_t rem = size - 1 - i;


        size_t maxFuture = rem * (rem + 1) / 2;

        size_t ub = std::min(i, remainingInv);

        size_t lb = (remainingInv > maxFuture) ? (remainingInv - maxFuture) : 0;

        if (lb > ub) {
            lb = std::min(lb, ub);
        }

        std::uniform_int_distribution<size_t> dist(lb, ub);
        size_t pos = dist(rng);

        result.insert(result.end() - static_cast<std::ptrdiff_t>(pos), arr[i]);

        remainingInv -= pos;
    }
    return result;
}
std::vector<int> DataGenerator::generateRem(size_t size, size_t remCount, int minValue, int maxValue, int k) {

    size_t lisLength = size - remCount;
    auto lis = generateSorted(lisLength, minValue, maxValue);
    auto rem = generateRandom(remCount, minValue, maxValue,  k );

    return mergeLISandRemovals(lis, rem);
}

std::vector<int> DataGenerator::mergeLISandRemovals(const std::vector<int>& lis,
                                                    const std::vector<int>& removals) {
    std::vector<int> result;
    result.reserve(lis.size() + removals.size());

    size_t lisIdx = 0, remIdx = 0;
    std::uniform_int_distribution<int> choice(0, 1);

    while (lisIdx < lis.size() || remIdx < removals.size()) {
        if (lisIdx < lis.size() && (remIdx >= removals.size() || choice(rng) == 0)) {
            result.push_back(lis[lisIdx++]);
        } else {
            result.push_back(removals[remIdx++]);
        }
    }
    return result;
}


std::vector<int> DataGenerator::generateHam(size_t size, size_t hamCount, int minValue, int maxValue) {


    std::vector<int> arr = generateSorted(size, minValue, maxValue);

    std::unordered_set<size_t> hamIndices;
    std::uniform_int_distribution<size_t> dist(0, size - 1);
    while (hamIndices.size() < hamCount) {
        hamIndices.insert(dist(rng));
    }

    for (size_t idx : hamIndices) {
        size_t swapIdx;
        do {
            swapIdx = dist(rng);
        } while (swapIdx == idx);
        std::swap(arr[idx], arr[swapIdx]);
    }

    return arr;
}

std::vector<std::vector<size_t>> DataGenerator::splitIntoCycles(
    std::vector<size_t>& indices, size_t cyclesCount) {
    std::shuffle(indices.begin(), indices.end(), rng);
    std::vector<std::vector<size_t>> cycles(cyclesCount);

    size_t idx = 0;
    for (size_t c = 0; c < cyclesCount; ++c) {
        size_t remaining = indices.size() - idx;
        size_t cyclesLeft = cyclesCount - c;
        size_t cycleSize = (remaining - (cyclesLeft - 1)) > 1
            ? std::uniform_int_distribution<size_t>(1, remaining - (cyclesLeft - 1))(rng)
            : 1;

        for (size_t j = 0; j < cycleSize; ++j) {
            cycles[c].push_back(indices[idx++]);
        }
    }
    return cycles;
}

void DataGenerator::rotateCycles(std::vector<std::vector<size_t>>& cycles) {
    for (auto& cycle : cycles) {
        if (cycle.size() > 1) {
            std::rotate(cycle.begin(), cycle.begin() + 1, cycle.end());
        }
    }
}

std::vector<int> DataGenerator::applyPermutation(
    const std::vector<int>& base,
    const std::vector<std::vector<size_t>>& cycles,
    const std::vector<size_t>& indices
) {
    std::vector<int> result(base.size());
    size_t idx = 0;
    for (const auto& cycle : cycles) {
        for (size_t src : cycle) {
            size_t dst = indices[idx++];
            result[dst] = base[src];
        }
    }
    return result;
}

std::vector<int> DataGenerator::generateExs(size_t size, size_t exchanges, int minValue, int maxValue) {

    auto base = generateSorted(size, minValue, maxValue);

    std::vector<size_t> indices(size);
    std::iota(indices.begin(), indices.end(), 0);

    size_t cyclesCount = size - exchanges;

    auto cycles = splitIntoCycles(indices, cyclesCount);

    rotateCycles(cycles);

    return applyPermutation(base, cycles, indices);
}

std::vector<int> DataGenerator::generateOsc(size_t size, size_t oscValue, int minValue, int maxValue) {
    if (size == 0) return {};
    if (oscValue > size - 1) oscValue = size - 1;

    std::vector<int> arr = generateSorted(size, minValue, maxValue);

    applyOscillation(arr, oscValue);

    return arr;
}

void DataGenerator::applyOscillation(std::vector<int>& arr, size_t oscValue) {
    size_t n = arr.size();
    if (n < 2 || oscValue == 0) return;

    size_t maxOsc = n - 1;
    if (oscValue > maxOsc) oscValue = maxOsc;

    std::vector<bool> directions(n - 1, true);

    std::unordered_set<size_t> changePos;
    std::uniform_int_distribution<size_t> dist(1, n - 2);

    while (changePos.size() < oscValue) {
        changePos.insert(dist(rng));
    }

    bool currentDir = true;
    for (size_t i = 0; i < n - 1; ++i) {
        directions[i] = currentDir;
        if (changePos.count(i) > 0) {
            currentDir = !currentDir;
        }
    }

    std::shuffle(arr.begin(), arr.end(), rng);
    std::sort(arr.begin(), arr.end());

    std::vector<int> result(n);
    size_t low = 0;
    size_t high = n - 1;

    result[0] = arr[low++];
    for (size_t i = 0; i < n - 1; ++i) {
        if (directions[i]) {
            result[i + 1] = arr[high--];
        } else {
            result[i + 1] = arr[low++];
        }
    }

    arr = std::move(result);
}

std::vector<int> DataGenerator::generateMax(
    size_t size, size_t maxDistance, int minValue, int maxValue)
{

    std::vector<int> arr = generateSorted(size, minValue, maxValue);


    std::uniform_int_distribution<size_t> dist(0, size - maxDistance - 1);
    size_t i = dist(rng);
    std::swap(arr[i], arr[i + maxDistance]);

    return arr;
}




