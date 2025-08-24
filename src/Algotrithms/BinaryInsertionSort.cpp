

#include "BinaryInsertionSort.h"

#include <chrono>
#include <iostream>


std::vector<int> BinaryInsertionSort:: sort(std::vector<int>& array) {
    std::vector<int> arr = array;
    swapCount = 0;

    auto start = std::chrono::high_resolution_clock::now();

    binaryInsertionSort(arr);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;

    std::cout << "Swaps: " << swapCount << std::endl;
    std::cout << "Runtime: " << runtime.count() << " seconds" << std::endl;

    return arr;
}

int BinaryInsertionSort:: binarySearch(const std::vector<int>& arr, int key, int low, int high) {
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] <= key)
            low = mid + 1;
        else
            high = mid;
    }
    return low;
}

void BinaryInsertionSort::  binaryInsertionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];


        int pos = binarySearch(arr, key, 0, i);

        for (int j = i; j > pos; --j)
            arr[j] = arr[j - 1];

        arr[pos] = key;
    }
}