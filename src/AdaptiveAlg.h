//
// Created by markg on 25.08.2025.
//

#ifndef ADAPTIVEALG_H
#define ADAPTIVEALG_H

#include <vector>
#include <array>
#include <chrono>
#include <stdexcept>

#endif //ADAPTIVEALG_H

class AdaptiveAlg {
public:
    std::vector<std::vector<int>> decomposeRuns(const std::vector<int>& A) {
        std::vector<std::vector<int>> runs;
        size_t n = A.size();
        if (n == 0) return runs;

        size_t start = 0;
        while (start < n) {
            size_t end = start + 1;
            while (end < n && A[end] >= A[end - 1]) ++end;
            runs.emplace_back(A.begin() + start, A.begin() + end);
            start = end;
        }
        return runs;
    }

    std::vector<int> createLongestRun(const std::vector<int>& A) {
        if (A.empty()) return {};

        // 1. Декомпозиция на возрастающие ряды
        std::vector<std::vector<int>> runs;
        size_t n = A.size();
        size_t start = 0;
        while (start < n) {
            size_t end = start + 1;
            while (end < n && A[end] >= A[end - 1]) ++end;
            runs.emplace_back(A.begin() + start, A.begin() + end);
            start = end;
        }

        // 2. Находим самый длинный ран
        size_t maxRunIndex = 0;
        for (size_t i = 1; i < runs.size(); ++i) {
            if (runs[i].size() > runs[maxRunIndex].size())
                maxRunIndex = i;
        }
        const auto& longestRun = runs[maxRunIndex];

        // 3. Находим минимум всего массива
        int minValue = *std::min_element(A.begin(), A.end());

        // 4. Создаем новый ран
        std::vector<int> newRun;
        newRun.push_back(minValue);

        // Добавляем элементы из исходного массива **до появления первого элемента длинного рана**
        int firstLongest = longestRun.front();
        for (int x : A) {
            if (x == minValue) continue;
            if (x <= firstLongest) newRun.push_back(x);
            if (x == firstLongest) break;
        }

        // 5. Объединяем с самым длинным ранoм
        newRun.insert(newRun.end(), longestRun.begin(), longestRun.end());

        return newRun;
    }


    std::vector<int> stats(const std::vector<int>& array) {
    std::vector<int> arr = array;

    auto start = std::chrono::high_resolution_clock::now();

    // вызываем твой собственный алгоритм сортировки
    arr = createLongestRun(arr);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = end - start;

    std::cout << "Runtime: " << runtime.count() << " seconds" << std::endl;

    return arr;
}

};