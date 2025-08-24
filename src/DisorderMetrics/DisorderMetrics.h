
#ifndef DISORDERMETRICS_H
#define DISORDERMETRICS_H
#include <algorithm>
#include <map>
#include <vector>


class DisorderMetrics {
public:
    DisorderMetrics() = default;

    int calculateRuns(int arr[], int size);

    long long mergeAndCollect(int arr[], int left, int mid, int right);
    long long mergeSortAndCollect(int arr[], int left, int right);
    long long calculateInversions(int arr[], int size);

    int calculateRem(int arr[], int n);

    int calculateOsc(int arr[], int n);

    int calculateDis(int arr[], int n);

    int calculateHam(int arr[], int n);

    int calculateMax(int arr[], int n);
};


#endif //DISORDERMETRICS_H
