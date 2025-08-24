

#include "DataGenerator.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

DataGenerator::DataGenerator()
    : rng(std::random_device{}())
{}

void DataGenerator::printArray(const std::vector<int>& arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i != arr.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

std::vector<int> DataGenerator::generateRandom(size_t size, int minValue, int maxValue) {
    std::uniform_int_distribution<int> dist(minValue, maxValue);
    std::vector<int> data(size);
    for (auto& val : data) {
        val = dist(rng);
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
    auto runLengths = generateRunLengths(size, runsCount);

    for (size_t i = 0; i < runsCount; ++i) {
        auto run = generateSorted(runLengths[i], minValue, maxValue);
        data.insert(data.end(), run.begin(), run.end());
    }

    return data;
}

std::vector<int> DataGenerator::generateRunLengths(size_t size, size_t runsCount) {
    if (runsCount == 0 || runsCount > size) {
        throw std::invalid_argument("runsCount must be between 1 and size");
    }

    std::vector<int> runLengths(runsCount, 1);
    size_t remaining = size - runsCount;

    std::uniform_int_distribution<size_t> dist(0, runsCount - 1);
    while (remaining > 0) {
        size_t idx = dist(rng);
        runLengths[idx]++;
        remaining--;
    }

    return runLengths;
}

std::vector<int> DataGenerator::generateInversions(size_t size, size_t invCount, int minValue, int maxValue) {
    if (size == 0) return {};
    if (invCount > size * (size - 1) / 2) {
        throw std::invalid_argument("Too many inversions for array size");
    }
    if (maxValue - minValue + 1 < static_cast<int>(size)) {
        throw std::invalid_argument("Range is too small for unique elements");
    }

    std::vector<int> arr = generateSorted(size, minValue, maxValue);

    std::vector<int> result;
    result.reserve(size);
    size_t remainingInv = invCount;

    for (size_t i = 0; i < size; ++i) {
        size_t maxPos = std::min(i, remainingInv);
        std::uniform_int_distribution<size_t> dist(0, maxPos);
        size_t pos = dist(rng);

        result.insert(result.end() - pos, arr[i]);
        remainingInv -= pos;
    }

    return result;
}

std::vector<int> DataGenerator::generateRem(size_t size, size_t remCount, int minValue, int maxValue) {
    if (remCount >= size) {
        throw std::invalid_argument("remCount must be less than size");
    }
    if (minValue > maxValue) {
        throw std::invalid_argument("minValue must be <= maxValue");
    }

    size_t lisLength = size - remCount;
    auto lis = generateSorted(lisLength, minValue, maxValue);
    auto rem = generateRandom(remCount, minValue, maxValue);

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
    if (size == 0) return {};
    if (hamCount > size) {
        throw std::invalid_argument("hamCount cannot be greater than array size");
    }


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
    if (exchanges > size - 1) {
        throw std::invalid_argument("Too many exchanges for given array size");
    }

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
    if (maxDistance >= size) {
        throw std::invalid_argument("maxDistance must be less than size");
    }


    std::vector<int> arr = generateSorted(size, minValue, maxValue);


    std::uniform_int_distribution<size_t> dist(0, size - maxDistance - 1);
    size_t i = dist(rng);
    std::swap(arr[i], arr[i + maxDistance]);

    return arr;
}




