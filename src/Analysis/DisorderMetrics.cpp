
#include "DisorderMetrics.h"
#include <deque>
#include <unordered_map>
#include <limits>



long long DisorderMetrics::calculateRuns(const std::vector<int>& arr) {
    if (arr.empty()) return 0;
    long long runsCount = 1;
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < arr[i - 1]) ++runsCount;
    }
    return runsCount;
}

long long DisorderMetrics::mergeAndCollect(std::vector<int>& arr, int left, int mid, int right) {
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    std::vector<int> leftPart(leftSize);
    std::vector<int> rightPart(rightSize);

    for (int i = 0; i < leftSize; ++i) leftPart[i] = arr[left + i];
    for (int j = 0; j < rightSize; ++j) rightPart[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    long long invCount = 0;

    while (i < leftSize && j < rightSize) {
        if (leftPart[i] <= rightPart[j]) {
            arr[k++] = leftPart[i++];
        } else {
            invCount += (leftSize - i);
            arr[k++] = rightPart[j++];
        }
    }

    while (i < leftSize) arr[k++] = leftPart[i++];
    while (j < rightSize) arr[k++] = rightPart[j++];
    return invCount;
}

long long DisorderMetrics::mergeSortAndCollect(std::vector<int>& arr, int left, int right) {
    long long invCount = 0;
    if (left < right) {
        int mid = left + (right - left) / 2;
        invCount += mergeSortAndCollect(arr, left, mid);
        invCount += mergeSortAndCollect(arr, mid + 1, right);
        invCount += mergeAndCollect(arr, left, mid, right);
    }
    return invCount;
}

long long DisorderMetrics::calculateInversions(const std::vector<int>& arr) {
    if (arr.size() < 2) return 0;
    std::vector<int> tmp = arr;
    return mergeSortAndCollect(tmp, 0, static_cast<int>(tmp.size()) - 1);
}


long long DisorderMetrics::calculateRem(const std::vector<int>& arr) {
    if (arr.empty()) return 0;
    std::vector<int> tail;
    tail.reserve(arr.size());
    for (int x : arr) {
        auto it = std::upper_bound(tail.begin(), tail.end(), x);
        if (it == tail.end()) tail.push_back(x);
        else *it = x;
    }
    return static_cast<long long>(tail.size());
}

long long DisorderMetrics::calculateOsc(const std::vector<int>& arr) {
    if (arr.size() < 3) return 0;
    long long count = 0;
    for (size_t i = 1; i + 1 < arr.size(); ++i) {
        bool isPeak = (arr[i - 1] < arr[i] && arr[i] > arr[i + 1]);
        bool isValley = (arr[i - 1] > arr[i] && arr[i] < arr[i + 1]);
        if (isPeak || isValley) ++count;
    }
    return count;
}

long long DisorderMetrics::calculateDis(const std::vector<int>& arr) {
    std::vector<int> sorted = arr;
    std::stable_sort(sorted.begin(), sorted.end());

    std::unordered_map<int, std::deque<int>> pos;
    pos.reserve(sorted.size());
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i) {
        pos[sorted[i]].push_back(i);
    }

    long long sum = 0;
    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        int idx = pos[arr[i]].front();
        pos[arr[i]].pop_front();
        sum += std::llabs(static_cast<long long>(idx) - i);
    }
    return sum;
}

long long DisorderMetrics::calculateHam(const std::vector<int>& arr) {
    std::vector<int> sorted = arr;
    std::sort(sorted.begin(), sorted.end());
    long long count = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] != sorted[i]) ++count;
    }
    return count;
}

long long DisorderMetrics::calculateMax(const std::vector<int>& arr) {
    std::vector<int> sorted = arr;
    std::stable_sort(sorted.begin(), sorted.end());

    std::unordered_map<int, std::deque<int>> pos;
    pos.reserve(sorted.size());
    for (int i = 0; i < static_cast<int>(sorted.size()); ++i) {
        pos[sorted[i]].push_back(i);
    }

    long long best = 0;
    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        int idx = pos[arr[i]].front();
        pos[arr[i]].pop_front();
        best = std::max(best, static_cast<long long>(std::abs(idx - i)));
    }
    return best;
}


double DisorderMetrics::normalizeInversions(long long invCount, long long n) {
    if (n < 2) return 0.0;
    long double nd = static_cast<long double>(n);
    long double denom = nd * (nd - 1.0L) / 2.0L;
    return static_cast<double>(static_cast<long double>(invCount) / denom);
}

double DisorderMetrics::normalizeRuns(long long runsCount, long long n) {
    if (n < 2) return 0.0;
    return static_cast<double>(
        (static_cast<long double>(runsCount) - 1.0L) /
        (static_cast<long double>(n) - 1.0L)
    );
}

double DisorderMetrics::normalizeRem(long long remCount, long long n) {
    if (n <= 0) return 0.0;
    return static_cast<double>(
        static_cast<long double>(remCount) / static_cast<long double>(n)
    );
}

double DisorderMetrics::normalizeOsc(long long oscCount, long long n) {
    if (n < 3) return 0.0;
    return static_cast<double>(
        static_cast<long double>(oscCount) / (static_cast<long double>(n) - 2.0L)
    );
}

double DisorderMetrics::normalizeDis(long long disSum, long long n) {
    if (n <= 1) return 0.0;
    long double nd = static_cast<long double>(n);
    long double denom = ((n % 2 == 0) ? (nd * nd / 2.0L) : ((nd * nd - 1.0L) / 2.0L));
    return static_cast<double>(static_cast<long double>(disSum) / denom);
}

double DisorderMetrics::normalizeHam(long long hamCount, long long n) {
    if (n <= 0) return 0.0;
    return static_cast<double>(
        static_cast<long double>(hamCount) / static_cast<long double>(n)
    );
}

double DisorderMetrics::normalizeMax(long long maxDisplacement, long long n) {
    if (n < 2) return 0.0;
    return static_cast<double>(
        static_cast<long double>(maxDisplacement) /
        (static_cast<long double>(n) - 1.0L)
    );
}