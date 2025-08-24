

#include "QuickSort.h"

#include <chrono>
#include <iostream>
#include <vector>



int QuickSort::partition(std::vector<int>& array, int low, int high) {
    int pivotIndex = (low + high) / 2;
    int pivot = array[pivotIndex];

    std::swap(array[pivotIndex], array[high]);
    swapCount++;

    int i = low;
    for (int j = low; j < high; j++) {
        if (array[j] <= pivot) {
            std::swap(array[i], array[j]);
            swapCount++;
            i++;
        }
    }

    std::swap(array[i], array[high]);
    swapCount++;

    return i;
}

void QuickSort::quickSort(std::vector<int>& array, int low, int high) {
    if (low < high) {
        int pivotIndex = partition(array, low, high);
        quickSort(array, low, pivotIndex - 1);
        quickSort(array, pivotIndex + 1, high);
    }
}

std::vector<int> QuickSort::sort(std::vector<int>& array) {
    std::vector<int> arr = array;
    swapCount = 0;

    auto start = std::chrono::high_resolution_clock::now();

    quickSort(arr, 0, arr.size() - 1);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;

    std::cout << "Swaps: " << swapCount << std::endl;
    std::cout << "Runtime: " << runtime.count() << " seconds" << std::endl;

    return arr;
}