

#include "MergeSort.h"

#include <chrono>
#include <iostream>
#include <vector>


template<typename T>
T plus_inf_sentinel() {
    return std::numeric_limits<T>::max();
}

std::vector<int> MergeSort::sort(std::vector<int>& array) {
    swapCount = 0;
    totalMergeCosts = 0;
    totalBufferCosts = 0;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> arr = array;
    if (!arr.empty())
        mergeSort(arr, 0, arr.size() - 1);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;

    std::cout << "Swaps: " << swapCount << std::endl;
    std::cout << "Merge costs: " << totalMergeCosts << std::endl;
    std::cout << "Buffer costs: " << totalBufferCosts << std::endl;
    std::cout << "Runtime: " << runtime.count() << " seconds" << std::endl;

    return arr;
}

void MergeSort:: merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1);
    std::vector<int> R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
        swapCount++;
    }

    while (i < n1) {
        arr[k++] = L[i++];
        swapCount++;
    }

    while (j < n2) {
        arr[k++] = R[j++];
        swapCount++;
    }
}


void MergeSort:: merge_runs_copy_half(std::vector<int>& A, int l, int m, int r, std::vector<int>& B) {
    int n1 = m - l;
    int n2 = r - m;
    if (COUNT_MERGE_COSTS) totalMergeCosts += (n1 + n2);

    if (n1 <= n2) {

        std::copy(A.begin() + l, A.begin() + m, B.begin());
        if (COUNT_MERGE_COSTS) totalBufferCosts += n1;

        int i = 0, j = m, k = l;
        while (i < n1 && j < r)
            A[k++] = (B[i] <= A[j]) ? B[i++] : A[j++];
        while (i < n1)
            A[k++] = B[i++];
    } else {
        std::copy(A.begin() + m, A.begin() + r, B.begin());
        if (COUNT_MERGE_COSTS) totalBufferCosts += n2;

        int i = m - 1, j = n2 - 1, k = r - 1;
        while (i >= l && j >= 0)
            A[k--] = (A[i] <= B[j]) ? B[j--] : A[i--];
        while (j >= 0)
            A[k--] = B[j--];
    }
}


void MergeSort:: merge_runs_basic(std::vector<int>& A, int l, int m, int r, std::vector<int>& B) {
    int n1 = m - l;
    int n2 = r - m;
    if (COUNT_MERGE_COSTS) totalMergeCosts += (n1 + n2);

    std::copy(A.begin() + l, A.begin() + r, B.begin());
    if (COUNT_MERGE_COSTS) totalBufferCosts += (n1 + n2);

    int i = 0, j = n1, k = l;
    while (i < n1 && j < n1 + n2)
        A[k++] = (B[i] <= B[j]) ? B[i++] : B[j++];
    while (i < n1)
        A[k++] = B[i++];
    while (j < n1 + n2)
        A[k++] = B[j++];
}


void MergeSort:: merge_runs_basic_sentinels(std::vector<int>& A, int l, int m, int r, std::vector<int>& B) {
    using T = int;
    int n1 = m - l;
    int n2 = r - m;
    if (COUNT_MERGE_COSTS) totalMergeCosts += (n1 + n2);

    for (int i = 0; i < n1; i++) B[i] = A[l + i];
    B[n1] = plus_inf_sentinel<T>();

    for (int j = 0; j < n2; j++) B[n1 + 1 + j] = A[m + j];
    B[n1 + n2 + 1] = plus_inf_sentinel<T>();

    if (COUNT_MERGE_COSTS) totalBufferCosts += (n1 + n2 + 2);

    int i = 0, j = n1 + 1, k = l;
    while (k < r)
        A[k++] = (B[i] <= B[j]) ? B[i++] : B[j++];
}

void MergeSort:: mergeSort(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}


