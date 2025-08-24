

#include "InsertionSort.h"

#include <chrono>

void InsertionSort:: insertionSort(std::vector<int> arr) {
    swapCount = 0;

    for (int i = 1; i < arr.size(); i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            swapCount++;
            j--;
        }
        arr[j + 1] = key;
        swapCount++;
    }

}

std::vector<int> InsertionSort::sort(std::vector<int> array) {
    std::vector<int> arr = array;
    swapCount = 0;

    auto start = std::chrono::high_resolution_clock::now();

    insertionSort(arr);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;

    std::cout << "Swaps: " << swapCount << std::endl;
    std::cout << "Runtime: " << runtime.count() << " seconds" << std::endl;

    return arr;
}

