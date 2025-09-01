#include <gtest/gtest.h>

// абсолютные пути, как просил
#include "C:/Users/markg/CLionProjects/DisorderMetrics/src/Data/DataGenerator.h"
#include "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/DisorderMetrics.h"

#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_map>

class DataGeneratorTest : public ::testing::Test {
protected:
    static constexpr int N = 10000;
    static constexpr int MINV = 0;
    static constexpr int MAXV = 10000;

    DisorderMetrics dm;

};


TEST_F(DataGeneratorTest, GenerateRuns_MatchesRequested) {
    DataGenerator g;

    std::vector<int> targets = {1, 2, 10, 100, 500, 1000};
    for (int runsTarget : targets) {
        auto arr = g.generateRuns(N, runsTarget, MINV, MAXV);
        ASSERT_EQ((int)arr.size(), N);
        EXPECT_EQ(dm.calculateRuns(arr), runsTarget);
    }
}









