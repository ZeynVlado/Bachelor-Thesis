

#ifndef EXPSETUPPER_H
#define EXPSETUPPER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <functional>

#include "DataGenerator.h"
#include "Sampler.h"

class ExperimentConfigurator {
public:

    struct Config {
        int n;
        int sampleSize;
        int minValue;
        int maxValue;
        std::string root;

        std::vector<std::string> clusterGroups { "sqrt", "2sqrt", "log2", "2log2", "smlLength" };

        std::function<int(const std::string&, int /*S*/)> clusterSizer =
            [](const std::string& g, int S){
                auto clamp = [&](int v){ return std::max(1, std::min(v, S)); };
                if (g == "sqrt")     return clamp((int)std::floor(std::sqrt((double)S)));
                if (g == "2sqrt")    return clamp(2 * (int)std::floor(std::sqrt((double)S)));
                if (g == "log2")     return clamp((int)std::floor(std::log2((double)S)));
                if (g == "2log2")    return clamp(2 * (int)std::floor(std::log2((double)S)));
                if (g == "smlLength")return S; // один кластер = весь сэмпл
                return clamp((int)std::floor(std::sqrt((double)S)));
            };

        std::vector<std::string> stratumGroups{
            "smlLength", "2 smlLength", "n div 4", "n div 10", "n div 20"
        };

        std::function<int(const std::string &, int /*n*/, int /*S*/)> stratumSizer =
            [](const std::string &g, int n, int S) {
                auto clamp = [&](int v){ return std::max(1, std::min(v, n)); };
                if (g == "smlLength")    return clamp(S);
                if (g == "2 smlLength")  return clamp(2*S);
                if (g == "n div 4")      return clamp(n/4);
                if (g == "n div 10")     return clamp(n/10);
                if (g == "n div 20")     return clamp(n/20);
                return clamp((int)std::floor(std::sqrt((double)n)));
            };
    };

    ExperimentConfigurator(int n, int sampleSize, int minValue, int maxValue, const std::string& root)
        : cfg_{n, sampleSize, minValue, maxValue, root} {}

    explicit ExperimentConfigurator(Config cfg) : cfg_(std::move(cfg)) {}

    void configure(int countPerSet,
                   const std::vector<int>& kValues,
                   const std::vector<int>& runsValues,
                   bool includePermutation = true,
                   bool includeRandomK     = true,
                   bool includeRuns        = true) const
    {
        if (includePermutation) {
            std::cout << "[EXP] permutation n=" << cfg_.n
                      << " count=" << countPerSet << "\n";
            generatePermutationSet(countPerSet);
        }

        if (includeRandomK) {
            for (int k : kValues) {
                std::cout << "[EXP] random_array n=" << cfg_.n
                          << " k=" << k
                          << " count=" << countPerSet << "\n";
                generateRandomSet(k, countPerSet);
            }
        }

        if (includeRuns) {
            for (int r : runsValues) {
                std::cout << "[EXP] run_array n=" << cfg_.n
                          << " runs=" << r
                          << " (" << runsLabel(cfg_.n, r) << ")"
                          << " count=" << countPerSet << "\n";
                generateRunsSet(r, countPerSet);
            }
        }

        std::cout << "[OK] experiment_data_input fully populated under: " << cfg_.root << "\n";
    }

    void generatePermutationSet(int count) const {
        DataGenerator gen;
        std::vector<std::vector<int>> arrays;
        arrays.reserve(count);
        for (int i = 0; i < count; ++i) {
            arrays.emplace_back(gen.generatePermutation(cfg_.n));
        }
        const std::string base = join(cfg_.root, join("permutation", toStr(cfg_.n)));
        saveArraysAndAllSamples(arrays, base);
    }

    void generateRandomSet(int k, int count) const {
        DataGenerator gen;
        std::vector<std::vector<int>> arrays; arrays.reserve(count);
        for (int i = 0; i < count; ++i) {
            arrays.emplace_back(gen.generateRandom(cfg_.n, cfg_.minValue, cfg_.maxValue, k));
        }
        const std::string base = join(cfg_.root,
                                      join("random_array", join(toStr(cfg_.n), join("k", toStr(k)))));
        saveArraysAndAllSamples(arrays, base);
    }

    void generateRunsSet(int runs, int count) const {
        DataGenerator gen;
        std::vector<std::vector<int>> arrays;
        arrays.reserve(count);

        int fails = 0;
        while ((int)arrays.size() < count) {
            try {
                auto arr = gen.generateRuns(cfg_.n, runs, cfg_.minValue, cfg_.maxValue);
                if ((int)arr.size() != cfg_.n) { ++fails; continue; }
                arrays.emplace_back(std::move(arr));
            } catch (...) { ++fails; continue; }
        }

        const std::string base = join(cfg_.root,
            join("run_array", join(toStr(cfg_.n), join("r", runsLabel(cfg_.n, runs)))));
        saveArraysAndAllSamples(arrays, base);
    }

    static std::vector<int> loadArrayByIndex(const std::string& csvPath, int index0) {
        std::ifstream f(csvPath);
        if (!f) throw std::runtime_error("Cannot open: " + csvPath);
        std::string line;
        for (int i = 0; i < index0; ++i) {
            if (!std::getline(f, line)) throw std::out_of_range("Index out of range for " + csvPath);
        }
        if (!std::getline(f, line)) throw std::out_of_range("Index out of range for " + csvPath);
        std::vector<int> out; out.reserve(1024);
        std::stringstream ss(line); std::string cell;
        while (std::getline(ss, cell, ',')) out.push_back(std::stoi(cell));
        return out;
    }

private:
    Config cfg_;

    static std::string toStr(int v) { return std::to_string(v); }

    static std::string join(const std::string& a, const std::string& b) {
        if (a.empty()) return b;
        const char sep = '/';
        if (a.back() == '/' || a.back() == '\\') return a + b;
        return a + sep + b;
    }

    static void ensureDir(const std::string& p) {
        std::filesystem::create_directories(p);
    }

    static void writeRowCSV(std::ofstream& ofs, const std::vector<int>& v) {
        for (size_t i = 0; i < v.size(); ++i) {
            ofs << v[i];
            if (i + 1 < v.size()) ofs << ',';
        }
        ofs << '\n';
    }

    static std::string runsLabel(int n, int runs) {
        const int sqrt_n  = std::max(1, (int)std::floor(std::sqrt((double)n)));
        const int log2_n  = std::max(1, (int)std::floor(std::log2((double)n)));
        if (runs == sqrt_n)      return "sqrt n";
        if (runs == 2*sqrt_n)    return "2sqrt n";
        if (runs == log2_n)      return "log2 n";
        if (runs == n/4)         return "n div 4";
        if (runs == n/10)        return "n div 10";
        if (runs == n/20)        return "n div 20";
        return "r_" + std::to_string(runs);
    }

    static std::string clusterFolderLabel(const std::string& key) {
        if (key == "sqrt")      return "sqrt smlLength";
        if (key == "2sqrt")     return "2sqrt smlLength";
        if (key == "log2")      return "log2 smlLength";
        if (key == "2log2")     return "2log2 smlLength";
        if (key == "smlLength") return "smlLength";
        return key; // fallback
    }

    static std::string stratumFolderLabel(const std::string& key) {
        return key;
    }

    void saveArraysAndAllSamples(const std::vector<std::vector<int>>& arrays,
                                 const std::string& baseDir) const
    {
        // 1) arrays.csv
        const std::string arraysCsv = join(baseDir, "arrays.csv");
        ensureDir(std::filesystem::path(arraysCsv).parent_path().string());
        {
            std::ofstream ofs(arraysCsv, std::ios::trunc);
            if (!ofs) throw std::runtime_error("Cannot open " + arraysCsv);
            for (const auto& v : arrays) writeRowCSV(ofs, v);
        }

        const std::string samplesRoot = join(join(baseDir, "samples"), "sqrt n");

        // Cluster sampling
        for (const auto& cg : cfg_.clusterGroups) {
            const int clSize = cfg_.clusterSizer(cg, cfg_.sampleSize);
            const std::string dir = join(join(samplesRoot, "cluster sampling"),
                                         clusterFolderLabel(cg));
            ensureDir(dir);
            const std::string csv = join(dir, "samples.csv");
            std::ofstream ofs(csv, std::ios::trunc);
            if (!ofs) throw std::runtime_error("Cannot open " + csv);

            for (const auto& baseArr : arrays) {
                Sampler sam(baseArr, cfg_.sampleSize);
                sam.setStrategy(SamplingStrategy::CLUSTER);
                sam.createSample(/*clusterSize*/ clSize, /*stratSize*/ 0);
                writeRowCSV(ofs, sam.getSample());
            }
        }

        // Stratified sampling
        for (const auto& sg : cfg_.stratumGroups) {
            const int strSize = cfg_.stratumSizer(sg, cfg_.n, cfg_.sampleSize);
            const std::string dir = join(join(samplesRoot, "stratified sampling"),
                                         stratumFolderLabel(sg));
            ensureDir(dir);
            const std::string csv = join(dir, "samples.csv");
            std::ofstream ofs(csv, std::ios::trunc);
            if (!ofs) throw std::runtime_error("Cannot open " + csv);

            for (const auto& baseArr : arrays) {
                Sampler sam(baseArr, cfg_.sampleSize);
                sam.setStrategy(SamplingStrategy::STRATIFIED);
                sam.createSample(/*clusterSize*/ 0, /*stratSize*/ strSize);
                writeRowCSV(ofs, sam.getSample());
            }
        }

        // Combined sampling
        for (const auto& sg : cfg_.stratumGroups) {
            const int strSize = cfg_.stratumSizer(sg, cfg_.n, cfg_.sampleSize);
            const std::string dir = join(join(samplesRoot, "combined sampling"),
                                         stratumFolderLabel(sg));
            ensureDir(dir);
            const std::string csv = join(dir, "samples.csv");
            std::ofstream ofs(csv, std::ios::trunc);
            if (!ofs) throw std::runtime_error("Cannot open " + csv);

            for (const auto& baseArr : arrays) {
                Sampler sam(baseArr, cfg_.sampleSize);
                sam.setStrategy(SamplingStrategy::COMBINED);
                sam.createSample(/*clusterSize*/ 0, /*stratSize*/ strSize);
                writeRowCSV(ofs, sam.getSample());
            }
        }

        std::cout << "[OK] arrays + samples saved under: " << baseDir << "\n";
    }
};

#endif //EXPSETUPPER_H
