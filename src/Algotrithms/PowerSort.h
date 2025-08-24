

#ifndef POWERSORT_H
#define POWERSORT_H
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>
#include <stack>


class PowerSort {

public:

const int minRunLen = 32;

    struct Run {
        int start;
        int end;
    };

    const Run NULL_RUN = {-1, -1};

    inline bool isNullRun(const Run& r) {
        return r.start == -1 && r.end == -1;
    }

    inline unsigned floor_log2(size_t n) {
        return (unsigned)std::floor(std::log2((double)n));
    }


    int extend_and_reverse_run_right(std::vector<int>& A, int start, int right) {
        int end = start;
        if (start >= right) return start;

        if (A[start + 1] < A[start]) {
            while (end < right && A[end + 1] < A[end]) ++end;
            std::reverse(A.begin() + start, A.begin() + end + 1);
        } else {
            while (end < right && A[end + 1] >= A[end]) ++end;
        }
        return end;
    }


    void insertionsort(std::vector<int>& A, int left, int right, int len) {
        for (int i = left + 1; i <= right; i++) {
            int key = A[i];
            int j = i - 1;
            while (j >= left && A[j] > key) {
                A[j + 1] = A[j];
                --j;
            }
            A[j + 1] = key;
        }
    }
    void merge_runs(std::vector<int>& A, int left, int mid, int right, std::vector<int>& buffer) {
        int n1 = mid - left;
        buffer.resize(std::max<int>(buffer.size(), n1));

        for (int i = 0; i < n1; i++) buffer[i] = A[left + i];

        int i = 0, j = mid, k = left;
        while (i < n1 && j < right) {
            if (buffer[i] <= A[j]) {
                A[k++] = buffer[i++];
            } else {
                A[k++] = A[j++];
            }
        }
        while (i < n1) {
            A[k++] = buffer[i++];
        }
    }

    using power_t = unsigned;

    power_t node_power_bitwise(size_t begin, size_t end,
                               size_t beginA, size_t beginB, size_t endB) {
        size_t n = end - begin;
        assert(n < (1ULL << 63));

        size_t l = (beginA - begin) + (beginB - begin);
        size_t r = (beginB - begin) + (endB - begin);

        power_t nCommonBits = 0;
        bool digitA = l >= n, digitB = r >= n;
        while (digitA == digitB) {
            ++nCommonBits;
            if (digitA) {
                l -= n;
                r -= n;
            }
            l *= 2;
            r *= 2;
            digitA = l >= n;
            digitB = r >= n;
        }
        return nCommonBits + 1;
    }
    void power_sort(std::vector<int>& A, int left, int right) {
        int n = right - left + 1;
        unsigned lgnPlus2 = floor_log2(n) + 2;

        std::vector<Run> runStack(lgnPlus2, NULL_RUN);
        assert(isNullRun(runStack[0]) && isNullRun(runStack[lgnPlus2 - 1]));
        unsigned top = 0;

        std::vector<int> buffer(n / 2);

        Run runA = {left, extend_and_reverse_run_right(A, left, right)};
        int lenA = runA.end - runA.start + 1;
        if (lenA < minRunLen) {
            runA.end = std::min(right, runA.start + minRunLen - 1);
            insertionsort(A, runA.start, runA.end, lenA);
        }

        while (runA.end < right) {
            Run runB = {runA.end + 1, extend_and_reverse_run_right(A, runA.end + 1, right)};
            int lenB = runB.end - runB.start + 1;
            if (lenB < minRunLen) {
                runB.end = std::min(right, runB.start + minRunLen - 1);
                insertionsort(A, runB.start, runB.end, lenB);
            }

            unsigned k = node_power_bitwise(left, right + 1, runA.start, runB.start, runB.end);
            assert(k != top);

            for (unsigned l = top; l > k; --l) {
                if (isNullRun(runStack[l])) continue;
                merge_runs(A, runStack[l].start, runStack[l].end + 1, runA.end + 1, buffer);
                runA.start = runStack[l].start;
                runStack[l] = NULL_RUN;
            }

            runStack[k] = runA;
            top = k;
            runA = runB;
        }

        assert(runA.end == right);
        for (unsigned l = top; l > 0; --l) {
            if (!isNullRun(runStack[l])) {
                merge_runs(A, runStack[l].start, runStack[l].end + 1, right + 1, buffer);
            }
        }
    }

};




#endif //POWERSORT_H
