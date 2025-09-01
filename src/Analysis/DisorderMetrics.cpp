

#include "DisorderMetrics.h"

#include <deque>
#include <unordered_map>
#include <limits>

int DisorderMetrics::calculateRuns(const std::vector<int>& arr) {
    if (arr.empty()) return 0;

    int runsCount = 1;

    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i - 1]) {
            runsCount++;
        }
    }

    return runsCount;
}

long long DisorderMetrics::mergeAndCollect(std::vector<int> arr, int left, int mid, int right) {
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    std::vector<int> leftPart(leftSize);
    std::vector<int> rightPart(rightSize);

    for (int i = 0; i < leftSize; i++) leftPart[i] = arr[left + i];
    for (int i = 0; i < rightSize; i++) rightPart[i] = arr[mid + 1 + i];

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

long long DisorderMetrics::mergeSortAndCollect(std::vector<int> arr, int left, int right) {
    long long invCount = 0;
    if (left < right) {
        int mid = left + (right - left) / 2;
        invCount += mergeSortAndCollect(arr, left, mid);
        invCount += mergeSortAndCollect(arr, mid + 1, right);
        invCount += mergeAndCollect(arr, left, mid, right);
    }
    return invCount;
}

long long DisorderMetrics::calculateInversions(std::vector<int> arr) {
    return mergeSortAndCollect(arr, 0, arr.size() - 1);
}

int DisorderMetrics::calculateRem(const std::vector<int> a) {
    if (a.empty()) return 0;
    std::vector<int> tail;
    for (int x : a) {
        auto it = std::upper_bound(tail.begin(), tail.end(), x);
        if (it == tail.end()) tail.push_back(x);
        else *it = x;
    }
    return static_cast<int>(tail.size());
}

int DisorderMetrics::calculateOsc(const std::vector<int> arr) {
    int count = 0;
    if (arr.size() < 3) return 0;

    for (size_t i = 1; i < arr.size() - 1; i++) {
        bool isPeak = (arr[i - 1] < arr[i] && arr[i] > arr[i + 1]);
        bool isValley = (arr[i - 1] > arr[i] && arr[i] < arr[i + 1]);
        if (isPeak || isValley) count++;
    }

    return count;
}


int DisorderMetrics::calculateDis(const std::vector<int> a) {
    std::vector<int> sorted = a;
    std::stable_sort(sorted.begin(), sorted.end());

    std::unordered_map<int, std::deque<int>> pos;
    pos.reserve(sorted.size());
    for (int i = 0; i < (int)sorted.size(); ++i) pos[sorted[i]].push_back(i);

    long long sum = 0;
    for (int i = 0; i < (int)a.size(); ++i) {
        int idx = pos[a[i]].front(); pos[a[i]].pop_front();
        sum += std::llabs((long long)idx - i);
    }
    return sum;
}


int DisorderMetrics::calculateHam(const std::vector<int> arr) {
    std::vector<int> sortedArr = arr;
    std::sort(sortedArr.begin(), sortedArr.end());

    int count = 0;
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] != sortedArr[i]) count++;
    }

    return count;
}

int DisorderMetrics::calculateMax(const std::vector<int> a) {
    std::vector<int> sorted = a;
    std::stable_sort(sorted.begin(), sorted.end());
    std::unordered_map<int, std::deque<int>> pos;
    pos.reserve(sorted.size());
    for (int i = 0; i < (int)sorted.size(); ++i) pos[sorted[i]].push_back(i);

    int best = 0;
    for (int i = 0; i < (int)a.size(); ++i) {
        int idx = pos[a[i]].front(); pos[a[i]].pop_front();
        best = std::max(best, std::abs(idx - i));
    }
    return best;
}


double DisorderMetrics::normalizeInversions(long long invCount, long long n) {
    if (n < 2) return 0.0;
    // n*(n-1)/2
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
    // even: n^2/2, odd: (n^2 - 1)/2
    long double nd = static_cast<long double>(n);
    long double denom = ( (n % 2 == 0)
        ? (nd * nd / 2.0L)
        : ((nd * nd - 1.0L) / 2.0L) );
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
