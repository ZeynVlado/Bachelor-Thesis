


#ifndef SAMPLER_H
#define SAMPLER_H
#include <stdlib.h>
#include <vector>

#include "SamplingStrategy.h"
#include <random>
#include <unordered_set>


class Sampler {

private:
    std::vector<int> arr;
    int sampleLength;
    SamplingStrategy strategy;
    std::vector<int> sample;


public:
    Sampler(const std::vector<int> inputArray, int sampleLength)
        : arr(inputArray), sampleLength(sampleLength) {}

    void createSample(int clusterSize, int stratSize) {
        sample.clear();

        switch (strategy) {

            case SamplingStrategy::STRATIFIED: {
                stratified_sampling(stratSize);
                break;
            }
            case SamplingStrategy::CLUSTER: {
                clusterSampling(clusterSize);
                break;
            }
            case SamplingStrategy::COMBINED: {
                combinedSampling(stratSize);
                break;
            }
            default:

                break;
        }
    }

    struct Cluster {
        int start;
        int end;
    };

    void stratified_sampling(int stratLength) {
        int n = arr.size();
        sample.clear();

        if (n == 0 || sampleLength <= 0 || stratLength <= 0) {
            return;
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        int num_strata = (n + stratLength - 1) / stratLength;

        int per_stratum = sampleLength / num_strata;
        int remainder   = sampleLength % num_strata;

        int current_start = 0;

        for (int s = 0; s < num_strata - 1; ++s) {
            int stratum_size = std::min(stratLength, n - current_start);
            int stratum_end  = current_start + stratum_size;

            int take = per_stratum + (remainder > 0 ? 1 : 0);
            if (remainder > 0) remainder--;

            if (take > 0 && stratum_size > 0) {
                std::unordered_set<int> chosen_indices;
                std::uniform_int_distribution<> dis(current_start, current_start + stratum_size - 1);

                while ((int)chosen_indices.size() < take) {
                    int idx = dis(gen);
                    chosen_indices.insert(idx);
                }

                std::vector<int> sorted_indices(chosen_indices.begin(), chosen_indices.end());
                std::sort(sorted_indices.begin(), sorted_indices.end());

                for (int idx : sorted_indices) {
                    sample.push_back(arr[idx]);
                }
            }

            current_start = stratum_end;
        }

        if (num_strata > 0) {
            int stratum_size = n - current_start;
            int need = sampleLength - (int)sample.size();

            if (need > 0 && stratum_size > 0) {
                fill_last_stratum(current_start, n, need, gen);
            }
        }
    }

    void clusterSampling(int clusterSize) {
        int n = arr.size();
        sample.clear();
        if (n == 0 || sampleLength <= 0 || clusterSize <= 0) return;

        std::random_device rd;
        std::mt19937 gen(rd());

        int clusterLength = std::min(clusterSize, sampleLength);

        std::vector<Cluster> clusters = generateClusters(n, clusterLength, gen);

        int total_selected = 0;

        for (size_t c = 0; c < clusters.size(); ++c) {
            const auto& cl = clusters[c];
            int cluster_size = cl.end - cl.start;

            if (total_selected + cluster_size > sampleLength) {
                continue;
            }

            for (int i = cl.start; i < cl.end; ++i) {
                sample.push_back(arr[i]);
            }
            total_selected += cluster_size;
        }

        int remaining = sampleLength - total_selected;
        if (remaining > 0 && !clusters.empty()) {
            fill_remaining_elements(clusters, n, remaining, gen);
        }
    }


    void combinedSampling(int stratSize) {
        sample.clear();

        const int n = static_cast<int>(arr.size());
        if (n == 0 || sampleLength <= 0 || stratSize <= 0) return;

        std::random_device rd;
        std::mt19937 gen(rd());

        const auto strataSizes  = computeStrataSizes(n, stratSize);
        const auto strataStarts = computeStrataStarts(strataSizes);

        auto clusterSizes = computeProportionalClusterSizes(strataSizes, n, sampleLength);

        distributeRemainder(clusterSizes, strataSizes, sampleLength);

        for (size_t s = 0; s < strataSizes.size(); ++s) {
            const int take  = clusterSizes[s];
            if (take <= 0) continue;

            const int start = strataStarts[s];
            const int size  = strataSizes[s];

            appendRandomClusterFromStratum(start, size, take, gen, sample);
            if ((int)sample.size() >= sampleLength) break;
        }

        if ((int)sample.size() > sampleLength) {
            sample.resize(sampleLength);
        }
    }




    std::vector<int>& getArray() { return arr; }
    int getSampleLength() { return sampleLength; }
    void setSampleLength(int sampleLength) { this->sampleLength = sampleLength; }
    void setArray( std::vector<int>& array ) { this->arr = array; }
    SamplingStrategy getStrategy() { return strategy; }
    void setStrategy(SamplingStrategy strategy) { this->strategy = strategy; }
    std::vector<int>& getSample() { return sample; }


private:

    bool intersects(const Cluster &a, const Cluster &b) {
        return !(a.end <= b.start || b.end <= a.start);
    }

    std::vector<Cluster> generateClusters(int n, int clusterLength, std::mt19937& gen) {
        std::vector<Cluster> clusters;

        int target_clusters = std::ceil(static_cast<double>(sampleLength) / clusterLength);

        while (clusters.size() < target_clusters) {

            std::uniform_int_distribution<> dis(0, n - 1);
            int start = dis(gen);
            int end = std::min(start + clusterLength, n);

            Cluster newCluster{start, end};

            bool collision = false;
            for (const auto& c : clusters) {
                if (intersects(c, newCluster)) {
                    collision = true;
                    break;
                }
            }

            if (!collision) {
                clusters.push_back(newCluster);
            }
        }


        std::sort(clusters.begin(), clusters.end(),
            [](const Cluster& a, const Cluster& b) {
                return a.start < b.start;
        });



        return clusters;
    }



    void fill_remaining_elements(const std::vector<Cluster>& clusters, int n, int remaining, std::mt19937 &gen) {
        if (remaining <= 0) return;

        std::vector<int> candidates;

        int start_after_last = clusters.back().end;
        int end_after_last = n;
        int interval_size = end_after_last - start_after_last;

        if (interval_size >= remaining) {


            std::uniform_int_distribution<> dis(start_after_last, end_after_last - 1);
            std::unordered_set<int> chosen;
            while ((int)chosen.size() < remaining) {
                chosen.insert(dis(gen));
            }

            candidates.assign(chosen.begin(), chosen.end());
            std::sort(candidates.begin(), candidates.end());

            for (int idx : candidates) {
                sample.push_back(arr[idx]);
            }
            return;
        }

        for (int i = start_after_last; i < end_after_last; i++) candidates.push_back(i);
        remaining -= interval_size;

        for (size_t i = 1; i < clusters.size() && remaining > 0; i++) {
            int gap_start = clusters[i-1].end;
            int gap_end   = clusters[i].start;
            int gap_size  = gap_end - gap_start;
            if (gap_size <= 0) continue;

            int take = std::min(remaining, gap_size);
            for (int j = 0; j < take; j++) candidates.push_back(gap_start + j);
            remaining -= take;
        }


        std::sort(candidates.begin(), candidates.end());
        for (int idx : candidates) {
            sample.push_back(arr[idx]);
        }
    }


    void fill_last_stratum(int start_idx, int end_idx, int need, std::mt19937 &gen) {
        if (need <= 0 || start_idx >= end_idx) {
            return;
        }

        int minIndex = start_idx;
        int maxIndex = end_idx - need;

        std::uniform_int_distribution<> dis(minIndex, maxIndex);
        int start = dis(gen);

        for (int i = 0; i < need; i++) {
            int idx = start + i;
            sample.push_back(arr[idx]);
        }
    }

    std::vector<int> computeStrataSizes(int n, int stratSize) const {
    const int num = (n + stratSize - 1) / stratSize; // ceil
    std::vector<int> sizes(num, 0);
    for (int i = 0; i < num; ++i) {
        const int start = i * stratSize;
        const int end   = std::min(start + stratSize, n);
        sizes[i] = end - start;
    }
    return sizes;
}

    std::vector<int> computeStrataStarts(const std::vector<int>& strataSizes) const {
    std::vector<int> starts;
    starts.reserve(strataSizes.size());
    int cursor = 0;
    for (int sz : strataSizes) {
        starts.push_back(cursor);
        cursor += sz;
    }
    return starts;
}

    std::vector<int> computeProportionalClusterSizes(const std::vector<int>& strataSizes,
                                                 int n, int sampleLen) const {
    std::vector<int> cls(strataSizes.size(), 0);
    int total = 0;
    for (size_t i = 0; i < strataSizes.size(); ++i) {

        const double share = (n > 0) ? (static_cast<double>(strataSizes[i]) / n * sampleLen) : 0.0;
        cls[i] = std::min(strataSizes[i], static_cast<int>(std::floor(share)));
        total += cls[i];
    }
    return cls;
}

    void distributeRemainder(std::vector<int>& clusterSizes,
                         const std::vector<int>& strataSizes,
                         int sampleLen) const {

    int current = 0;
    for (int v : clusterSizes) current += v;
    int remaining = sampleLen - current;
    if (remaining <= 0) return;

    std::vector<int> idx(clusterSizes.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](int a, int b){
        if (strataSizes[a] != strataSizes[b]) return strataSizes[a] > strataSizes[b];
        return a < b;
    });

    for (int id : idx) {
        if (remaining == 0) break;
        const int canTake = strataSizes[id] - clusterSizes[id];
        const int add = std::min(canTake, remaining);
        if (add > 0) {
            clusterSizes[id] += add;
            remaining -= add;
        }
    }
}

    void appendRandomClusterFromStratum(int stratumStart, int stratumSize, int take,
                                    std::mt19937& gen, std::vector<int>& out) const {
    if (take <= 0 || stratumSize <= 0) return;
    take = std::min(take, stratumSize);
    const int maxStart = stratumSize - take;
    std::uniform_int_distribution<int> dis(0, maxStart);
    const int start = stratumStart + dis(gen);
    out.reserve(out.size() + take);
    for (int j = 0; j < take; ++j) out.push_back(arr[start + j]);
}


};

#endif //SAMPLER_H
