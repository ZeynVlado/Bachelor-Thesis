//
// Created by markg on 01.09.2025.
//

#ifndef DISORDERMETRICSTEST_H
#define DISORDERMETRICSTEST_H

#include "C:/Users/markg/CLionProjects/DisorderMetrics/cmake-build-debug/_deps/googletest-src/googletest/include/gtest/gtest.h"

#include <vector>
#include <algorithm>
#include <deque>
#include <unordered_map>
#include "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/DisorderMetrics.h"

class DisorderMetricsTest : public ::testing::Test {
protected:
    DisorderMetrics dm;
};

// Reference helper functions for validation

// Reference calculation for runs
static long long bruteRuns(const std::vector<int>& a) {
    if (a.empty()) return 0;
    long long runs = 1;
    for (size_t i = 1; i < a.size(); ++i)
        if (a[i] < a[i - 1]) ++runs;
    return runs;
}

// Reference O(n²) inversion count
static long long bruteInversions(const std::vector<int>& a) {
    long long c = 0;
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = i + 1; j < a.size(); ++j)
            if (a[i] > a[j]) ++c;
    return c;
}

// Helper for stable positions used in Dis/Max metrics
static std::unordered_map<int, std::deque<int>> buildStablePositions(const std::vector<int>& a) {
    std::vector<int> sorted = a;
    std::stable_sort(sorted.begin(), sorted.end());
    std::unordered_map<int, std::deque<int>> pos;
    pos.reserve(sorted.size());
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i)
        pos[sorted[i]].push_back(i);
    return pos;
}

// Reference calculation for Dis metric
static long long bruteDis(const std::vector<int>& a) {
    auto pos = buildStablePositions(a);
    long long sum = 0;
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        int idx = pos[a[i]].front();
        pos[a[i]].pop_front();
        sum += std::llabs(static_cast<long long>(idx) - i);
    }
    return sum;
}

// Reference calculation for Ham metric
static long long bruteHam(const std::vector<int>& a) {
    std::vector<int> b = a;
    std::sort(b.begin(), b.end());
    long long cnt = 0;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) ++cnt;
    return cnt;
}

// Reference calculation for Max displacement
static long long bruteMaxDisp(const std::vector<int>& a) {
    auto pos = buildStablePositions(a);
    long long best = 0;
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        int idx = pos[a[i]].front();
        pos[a[i]].pop_front();
        best = std::max(best, static_cast<long long>(std::abs(idx - i)));
    }
    return best;
}

// Reference calculation for Rem metric (LIS length)
static long long bruteRem(const std::vector<int>& a) {
    if (a.empty()) return 0;
    std::vector<int> tail;
    for (int x : a) {
        auto it = std::upper_bound(tail.begin(), tail.end(), x);
        if (it == tail.end()) tail.push_back(x);
        else *it = x;
    }
    return static_cast<long long>(tail.size());
}

// Tests for calculateRuns

TEST_F(DisorderMetricsTest, Runs_Empty) {
    EXPECT_EQ(dm.calculateRuns({}), 0);
}

TEST_F(DisorderMetricsTest, Runs_SingleElement) {
    EXPECT_EQ(dm.calculateRuns({42}), 1);
}

TEST_F(DisorderMetricsTest, Runs_SortedAscending) {
    EXPECT_EQ(dm.calculateRuns({1,2,3,4,5}), 1);
}

TEST_F(DisorderMetricsTest, Runs_SortedDescending) {
    EXPECT_EQ(dm.calculateRuns({5,4,3,2,1}), 5);
}

TEST_F(DisorderMetricsTest, Runs_WithDuplicates) {
    EXPECT_EQ(dm.calculateRuns({1,1,2,2,2,3,3}), 1);
}

TEST_F(DisorderMetricsTest, Runs_ZigZagPattern) {
    EXPECT_EQ(dm.calculateRuns({1,3,2,4,3,5}), 3);
}

TEST_F(DisorderMetricsTest, Runs_AgainstReference) {
    std::vector<int> a{2,2,1,3,3,2,4,5,5,4};
    EXPECT_EQ(dm.calculateRuns(a), bruteRuns(a));
}

// Tests for calculateInversions

TEST_F(DisorderMetricsTest, Inv_Empty) {
    EXPECT_EQ(dm.calculateInversions({}), 0);
}

TEST_F(DisorderMetricsTest, Inv_SortedAscending) {
    EXPECT_EQ(dm.calculateInversions({1,2,3,4,5}), 0);
}

TEST_F(DisorderMetricsTest, Inv_SortedDescending) {
    EXPECT_EQ(dm.calculateInversions({5,4,3,2,1}), 10); // n*(n-1)/2
}

TEST_F(DisorderMetricsTest, Inv_WithDuplicates) {
    // Expected inversions: (3,2), (3,1), (2,1), (3,1) = 4
    EXPECT_EQ(dm.calculateInversions({1,3,2,3,1}), 4);
}

TEST_F(DisorderMetricsTest, Inv_RandomAgainstReference) {
    std::vector<int> a{8,1,2,9,5,3,7,6,4,4};
    EXPECT_EQ(dm.calculateInversions(a), bruteInversions(a));
}

TEST_F(DisorderMetricsTest, Inv_InputNotMutated) {
    std::vector<int> a{3,1,2};
    auto before = a;
    (void)dm.calculateInversions(a);
    EXPECT_EQ(a, before);
}

// Tests for calculateRem

TEST_F(DisorderMetricsTest, Rem_BasicCases) {
    EXPECT_EQ(dm.calculateRem({}), 0);
    EXPECT_EQ(dm.calculateRem({1}), 1);
    EXPECT_EQ(dm.calculateRem({1,2,3}), 3);
    EXPECT_EQ(dm.calculateRem({3,2,1}), 1);
}

TEST_F(DisorderMetricsTest, Rem_WithDuplicates) {
    std::vector<int> a{1,3,3,2,2,2,4};
    EXPECT_EQ(dm.calculateRem(a), bruteRem(a));
}

TEST_F(DisorderMetricsTest, Rem_RandomAgainstReference) {
    std::vector<int> a{2,5,3,4,3,6,7,6,8};
    EXPECT_EQ(dm.calculateRem(a), bruteRem(a));
}

// Tests for calculateOsc

TEST_F(DisorderMetricsTest, Osc_ShortSequences) {
    EXPECT_EQ(dm.calculateOsc({}), 0);
    EXPECT_EQ(dm.calculateOsc({1}), 0);
    EXPECT_EQ(dm.calculateOsc({1,2}), 0);
}

TEST_F(DisorderMetricsTest, Osc_PeaksAndValleys) {

    std::vector<int> a{1,3,2,4,3,5,4};
    EXPECT_EQ(dm.calculateOsc(a), 5);
}

TEST_F(DisorderMetricsTest, Osc_FlatSequence) {
    std::vector<int> a{1,1,1,1};
    EXPECT_EQ(dm.calculateOsc(a), 0);
}

// Tests for calculateDis

TEST_F(DisorderMetricsTest, Dis_AgainstReference) {
    std::vector<int> a{3,1,2,1};
    EXPECT_EQ(dm.calculateDis(a), bruteDis(a));
}

TEST_F(DisorderMetricsTest, Dis_WithDuplicates) {
    std::vector<int> a{2,2,1,1,3};
    EXPECT_EQ(dm.calculateDis(a), bruteDis(a));
}

// Tests for calculateHam

TEST_F(DisorderMetricsTest, Ham_BasicCases) {
    EXPECT_EQ(dm.calculateHam({}), 0);
    EXPECT_EQ(dm.calculateHam({1}), 0);
    EXPECT_EQ(dm.calculateHam({1,2,3}), 0);
    EXPECT_EQ(dm.calculateHam({3,2,1}), 2);
}

TEST_F(DisorderMetricsTest, Ham_AgainstReference) {
    std::vector<int> a{2,1,2,3,1};
    EXPECT_EQ(dm.calculateHam(a), bruteHam(a));
}

// Tests for calculateMax

TEST_F(DisorderMetricsTest, Max_AgainstReference) {
    std::vector<int> a{3,1,2,1};
    EXPECT_EQ(dm.calculateMax(a), bruteMaxDisp(a));
}

TEST_F(DisorderMetricsTest, Max_WithDuplicates) {
    std::vector<int> a{2,2,1,3,1};
    EXPECT_EQ(dm.calculateMax(a), bruteMaxDisp(a));
}

#endif //DISORDERMETRICSTEST_H
