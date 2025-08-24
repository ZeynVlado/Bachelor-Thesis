

#include "DisorderMetrics.h"


int DisorderMetrics::calculateRuns(int arr[], int size) {
    if (size == 0) return 0;

    int runsCount = 1;

    for (int i = 1; i < size; i++) {
        if (arr[i] <= arr[i - 1]) {
            runsCount++;
        }
    }

    return runsCount;
}

long long DisorderMetrics::mergeAndCollect(int arr[], int left, int mid, int right) {
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    int* leftPart = new int[leftSize];
    int* rightPart = new int[rightSize];

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

    delete[] leftPart;
    delete[] rightPart;

    return invCount;
}

long long DisorderMetrics::mergeSortAndCollect(int arr[], int left, int right) {
    long long invCount = 0;
    if (left < right) {
        int mid = left + (right - left) / 2;
        invCount += mergeSortAndCollect(arr, left, mid);
        invCount += mergeSortAndCollect(arr, mid + 1, right);
        invCount += mergeAndCollect(arr, left, mid, right);
    }
    return invCount;
}

long long DisorderMetrics::calculateInversions(int arr[], int size) {
    return mergeSortAndCollect(arr, 0, size - 1);
}


int DisorderMetrics::calculateRem(int arr[], int n) {
    if (n == 0) return 0;

    std::vector<int> result;
    result.push_back(arr[0]);

    for (int i = 1; i < n; i++) {
        if (result.back() <= arr[i]) {
            result.push_back(arr[i]);
        }
    }

    return static_cast<int>(result.size());
}

int DisorderMetrics::calculateOsc(int arr[], int n) {
    int count = 0;

    for (int i = 1; i < n - 1; i++) {
        bool isPeak = (arr[i - 1] < arr[i] && arr[i] > arr[i + 1]);
        bool isValley = (arr[i - 1] > arr[i] && arr[i] < arr[i + 1]);

        if (isPeak || isValley) {
            count++;
        }
    }

    return count;
}

int DisorderMetrics::calculateDis(int arr[], int n) {
    std::vector<int> sortedArr(arr, arr + n);
    std::sort(sortedArr.begin(), sortedArr.end());

    int displacementSum = 0;

    for (int i = 0; i < n; i++) {
        auto it = std::find(sortedArr.begin(), sortedArr.end(), arr[i]);
        int sortedPos = std::distance(sortedArr.begin(), it);
        displacementSum += std::abs(sortedPos - i);
    }

    return displacementSum;
}

int DisorderMetrics::calculateHam(int arr[], int n) {
    std::vector<int> sortedArr(arr, arr + n);
    std::sort(sortedArr.begin(), sortedArr.end());

    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] != sortedArr[i]) {
            count++;
        }
    }

    return count;
}

int DisorderMetrics::calculateMax(int arr[], int n) {
    std::vector<int> sortedArr(arr, arr + n);
    std::sort(sortedArr.begin(), sortedArr.end());

    int maxDisplacement = 0;

    for (int i = 0; i < n; i++) {

        auto it = std::find(sortedArr.begin(), sortedArr.end(), arr[i]);
        int sortedPos = std::distance(sortedArr.begin(), it);

        int displacement = std::abs(sortedPos - i);
        maxDisplacement = std::max(maxDisplacement, displacement);
    }

    return maxDisplacement;
}
