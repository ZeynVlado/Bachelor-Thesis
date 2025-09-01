
#ifndef DISORDERMETRICS_H
#define DISORDERMETRICS_H
#include <algorithm>
#include <vector>


class DisorderMetrics {
public:
    DisorderMetrics() = default;

    int calculateRuns(const std::vector<int>& arr);

    long long mergeAndCollect(std::vector<int> arr, int left, int mid, int right);
    long long mergeSortAndCollect(std::vector<int> arr, int left, int right);
    long long calculateInversions(std::vector<int> arr);

    int calculateRem(const std::vector<int> arr);

    int calculateOsc(const std::vector<int> arr);

    int calculateDis(const std::vector<int> arr);

    int calculateHam(const std::vector<int> arr);

    int calculateMax(const std::vector<int> arr);


    double normalizeInversions(long long invCount, long long n);      // invCount ∈ [0, n*(n-1)/2]
    double normalizeRuns(long long runsCount, long long n);           // runsCount ∈ [1, n]
    double normalizeRem(long long remCount, long long n);             // remCount ∈ [0, n]
    double normalizeOsc(long long oscCount, long long n);             // oscCount ∈ [0, n-2]
    double normalizeDis(long long disSum, long long n);               // disSum ≤ max for reverse: ⌊n^2/2⌋
    double normalizeHam(long long hamCount, long long n);             // hamCount ∈ [0, n]
    double normalizeMax(long long maxDisplacement, long long n);      // maxDisplacement ∈ [0, n-1]
};

#endif //DISORDERMETRICS_H
