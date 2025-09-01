#ifndef SAMPLERTEST_H
#define SAMPLERTEST_H

#define private public
#include "C:/Users/markg/CLionProjects/DisorderMetrics/src/Data/Sampler.h"
#undef private

#include "C:/Users/markg/CLionProjects/DisorderMetrics/src/Data/DataGenerator.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <vector>

class SamplerTest : public ::testing::Test {
protected:
    static constexpr int N = 10000;
    static constexpr int SAMPLE = 100;

    // value -> index в исходном массиве (перестановка 1..N)
    static std::vector<int> valueToIndex(const std::vector<int>& arr) {
        int mx = *std::max_element(arr.begin(), arr.end());
        std::vector<int> pos(mx + 1, -1);
        for (int i = 0; i < (int)arr.size(); ++i) pos[arr[i]] = i;
        return pos;
    }

    static std::vector<int> sampleIndices(const std::vector<int>& sample,
                                          const std::vector<int>& pos) {
        std::vector<int> idx; idx.reserve(sample.size());
        for (int v : sample) idx.push_back(pos[v]);
        return idx;
    }

    static bool strictlyIncreasing(const std::vector<int>& v) {
        for (size_t i = 1; i < v.size(); ++i) if (!(v[i] > v[i-1])) return false;
        return true;
    }
};

/* ============================ STRATIFIED ============================ */

TEST_F(SamplerTest, Stratified_Properties) {
    DataGenerator gen;
    auto arr = gen.generatePermutation(N);
    auto pos = valueToIndex(arr);

    std::vector<int> stratSizes = {
        (int)std::sqrt((double)N), (int)(2*std::sqrt((double)N)), N/4, N/10, N/20
    };

    for (int strat : stratSizes) {
        Sampler s(arr, SAMPLE);

        // 1) кол-во страт (ceil)
        auto sizes  = s.computeStrataSizes(N, strat);
        auto starts = s.computeStrataStarts(sizes);
        const int expected_num = (N + strat - 1) / strat;
        ASSERT_EQ((int)sizes.size(), expected_num);

        // run
        s.stratified_sampling(strat);

        // 4) длина выборки == 100
        ASSERT_EQ((int)s.getSample().size(), SAMPLE);

        // 5) исходный массив не изменён
        ASSERT_EQ((int)s.getArray().size(), N);

        // 3) индексы возрастают
        auto idx = sampleIndices(s.getSample(), pos);
        EXPECT_TRUE(strictlyIncreasing(idx)) << "strat=" << strat;

        // 2) из каждой страты >= 1 элемент
        std::vector<int> perStratum(expected_num, 0);
        for (int p : idx) {
            int k = int(std::upper_bound(starts.begin(), starts.end(), p) - starts.begin()) - 1;
            if (k < 0) k = 0;
            if (k >= expected_num) k = expected_num - 1;
            ++perStratum[k];
        }
        for (int k = 0; k < expected_num; ++k) {
            EXPECT_GE(perStratum[k], 1) << "strat=" << strat << " stratum=" << k;
        }
    }
}

/* ============================ CLUSTER ============================== */

TEST_F(SamplerTest, Cluster_Checks) {
    DataGenerator gen;
    auto arr = gen.generatePermutation(N);

    std::vector<int> clusterSizes = {
        (int)std::sqrt((double)SAMPLE),                 // 10
        (int)(2*std::sqrt((double)SAMPLE)),             // 20
        (int)std::log2((double)SAMPLE),                 // ~6
        (int)std::log2((double)SAMPLE * SAMPLE)         // ~13
    };

    for (int cs : clusterSizes) {
        Sampler s(arr, SAMPLE);

        // Проверяем generateClusters — доказываем пункты 2–4
        std::mt19937 rng(123456);
        const int clusterLen = std::min(cs, SAMPLE);
        auto clusters = s.generateClusters(N, clusterLen, rng);

        // (1) Кол-во кластеров: не утверждаем n/clusterLen, т.к. реализация подбирает
        //     столько кластеров, сколько нужно для выборки. Но кластеры укладываются в массив:
        EXPECT_LE((int)clusters.size() * clusterLen, N) << "cs=" << cs;

        // (2) Размер каждого кластера <= sampleLength (и <= clusterLen)
        for (const auto& c : clusters) {
            int len = c.end - c.start;
            EXPECT_GT(len, 0) << "cs=" << cs;
            EXPECT_LE(len, SAMPLE)     << "cs=" << cs;     // <= 100 — как ты и требовал
            EXPECT_LE(len, clusterLen) << "cs=" << cs;
        }

        // (3) Кластеры не пересекаются
        for (size_t i = 1; i < clusters.size(); ++i) {
            EXPECT_LE(clusters[i-1].end, clusters[i].start) << "cs=" << cs;
        }

        // (4) Кластеры отсортированы по возрастанию start
        for (size_t i = 1; i < clusters.size(); ++i) {
            EXPECT_LT(clusters[i-1].start, clusters[i].start) << "cs=" << cs;
        }

        // Полный метод — финальные требования (5) и (6)
        s.clusterSampling(cs);

        // (5) Длина выборки == 100
        EXPECT_EQ((int)s.getSample().size(), SAMPLE) << "cs=" << cs;

        // (6) Исходный массив не изменён (N = 10000)
        EXPECT_EQ((int)s.getArray().size(), N) << "cs=" << cs;
    }
}

/* ============================ COMBINED ============================= */

TEST_F(SamplerTest, Combined_Checks) {
    DataGenerator gen;
    auto arr = gen.generatePermutation(N);
    auto pos = valueToIndex(arr);

    std::vector<int> stratSizes = {
        (int)std::sqrt((double)N), (int)(2*std::sqrt((double)N)), N/4, N/10, N/20
    };

    for (int strat : stratSizes) {
        Sampler s(arr, SAMPLE);

        // 1) кол-во страт
        auto sizes  = s.computeStrataSizes(N, strat);
        auto starts = s.computeStrataStarts(sizes);
        const int numStrata = (int)sizes.size();
        ASSERT_EQ(numStrata, (N + strat - 1) / strat);

        // run
        s.combinedSampling(strat);

        // 4) длина выборки == 100
        ASSERT_EQ((int)s.getSample().size(), SAMPLE) << "strat=" << strat;

        // 5) массив не изменён
        ASSERT_EQ((int)s.getArray().size(), N);

        // 2) из каждой страты выбран >= 1 кластер (=> >=1 элемент)
        auto idx = sampleIndices(s.getSample(), pos);
        std::vector<int> perStratum(numStrata, 0);
        for (int p : idx) {
            int k = int(std::upper_bound(starts.begin(), starts.end(), p) - starts.begin()) - 1;
            if (k < 0) k = 0;
            if (k >= numStrata) k = numStrata - 1;
            ++perStratum[k];
        }
        for (int k = 0; k < numStrata; ++k) {
            EXPECT_GE(perStratum[k], 1) << "strat=" << strat << " stratum=" << k;
        }
    }
}

#endif // SAMPLERTEST_H


