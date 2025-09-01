

#ifndef CHARTBUILDER_H
#define CHARTBUILDER_H
#include <filesystem>
#include <fstream>
#include <vector>
#include "DisorderMetrics.h"

#include "../Data/Sampler.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <system_error>


namespace fs = std::filesystem;

class Evaluator {
public:
    Evaluator()  = default;
    ~Evaluator() = default;

    void prepareOutputStructure(const std::string& inputRoot,
                                const std::string& outputRoot) const
    {
        if (!fs::exists(inputRoot)) {
            throw std::runtime_error("Input root does not exist: " + inputRoot);
        }
        ensureDir(outputRoot);

        for (const auto& entry : fs::recursive_directory_iterator(inputRoot)) {
            if (entry.is_directory()) {
                const fs::path rel     = fs::relative(entry.path(), inputRoot);
                const fs::path outDir  = fs::path(outputRoot) / rel;
                ensureDir(outDir.string());

                if (hasFile(entry.path(), "samples.csv")) {
                    const fs::path sampleMetrics = outDir / "sample_metrics.csv";
                    createFileIfNotExists(sampleMetrics.string());
                }
            } else if (entry.is_regular_file() && entry.path().filename() == "arrays.csv") {
                const fs::path outDir        = fs::path(outputRoot) /
                                               fs::relative(entry.path().parent_path(), inputRoot);
                const fs::path arraysMetrics = outDir / "arrays_metrics.csv";
                createFileIfNotExists(arraysMetrics.string());
            }
        }

        std::cout << "[OK] Prepared output structure at: " << outputRoot << "\n";
    }

    void evaluateAll(const std::string& inputRoot,
                     const std::string& outputRoot) const
    {
        if (!fs::exists(inputRoot)) {
            throw std::runtime_error("Input root does not exist: " + inputRoot);
        }
        ensureDir(outputRoot);

        for (const auto& entry : fs::recursive_directory_iterator(inputRoot)) {
            const fs::path rel = fs::relative(entry.path(), inputRoot);
            const fs::path out = fs::path(outputRoot) / rel;


            if (entry.is_regular_file() && entry.path().filename() == "arrays.csv") {
                ensureDir(out.parent_path().string());
                evaluateCSVtoNormMetrics(
                    entry.path().string(),
                    (out.parent_path() / "arrays_metrics.csv").string()
                );
                continue;
            }


            if (entry.is_directory() && hasFile(entry.path(), "samples.csv")) {
                ensureDir(out.string());
                evaluateCSVtoNormMetrics(
                    (entry.path() / "samples.csv").string(),
                    (out / "sample_metrics.csv").string()
                );
                continue;
            }
        }

        std::cout << "[OK] Full evaluation finished. Output at: " << outputRoot << "\n";
    }

private:

    static void ensureDir(const std::string& path) {
        std::error_code ec;
        fs::create_directories(path, ec);
        if (ec) throw std::runtime_error("Failed to create directories: " + path + " (" + ec.message() + ")");
    }

    static bool hasFile(const fs::path& dir, const std::string& filename) {
        if (!fs::exists(dir) || !fs::is_directory(dir)) return false;
        for (const auto& it : fs::directory_iterator(dir)) {
            if (it.is_regular_file() && it.path().filename() == filename) return true;
        }
        return false;
    }

    static void createFileIfNotExists(const std::string& path) {
        if (!fs::exists(path)) {
            std::ofstream ofs(path);
            if (!ofs) throw std::runtime_error("Cannot create file: " + path);
        }
    }


    static bool readNextRow(std::istream& is, std::vector<int>& out) {
        out.clear();
        std::string line;
        if (!std::getline(is, line)) return false;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            out.push_back(cell.empty() ? 0 : std::stoi(cell));
        }
        return true;
    }

    static void writeHeaderNorm(std::ofstream& ofs) {
        ofs << "n,"
            << "inv_norm,"
            << "runs_norm,"
            << "rem_norm,"
            << "osc_norm,"
            << "dis_norm,"
            << "ham_norm"
            << "\n";
    }

    static void evaluateCSVtoNormMetrics(const std::string& inputCsv,
                                         const std::string& outputCsv)
    {
        std::ifstream ifs(inputCsv);
        if (!ifs) {
            throw std::runtime_error("Cannot open input csv: " + inputCsv);
        }

        std::ofstream ofs(outputCsv, std::ios::trunc);
        if (!ofs) {
            throw std::runtime_error("Cannot open output csv: " + outputCsv);
        }

        writeHeaderNorm(ofs);

        std::vector<int> row;
        while (readNextRow(ifs, row)) {
            if (row.empty()) continue;
            writeNormMetricsLine(ofs, row);
        }
    }

    static void writeNormMetricsLine(std::ofstream& ofs, const std::vector<int>& arr) {

        std::vector<int> tmp = arr;

        DisorderMetrics dm;

        const int n = static_cast<int>(arr.size());

        const int runsRaw     = dm.calculateRuns(arr);
        const double runsNorm = dm.normalizeRuns(runsRaw, n);

        const long long invRaw = dm.calculateInversions(tmp); // tmp портится
        const double invNorm   = dm.normalizeInversions(invRaw, n);

        const int remRaw     = dm.calculateRem(arr);
        const double remNorm = dm.normalizeRem(remRaw, n);

        const int oscRaw     = dm.calculateOsc(arr);
        const double oscNorm = dm.normalizeOsc(oscRaw, n);

        const int disRaw     = dm.calculateDis(arr);
        const double disNorm = dm.normalizeDis(disRaw, n);

        const int hamRaw     = dm.calculateHam(arr);
        const double hamNorm = dm.normalizeHam(hamRaw, n);

        ofs << n << ","
            << invNorm  << ","
            << runsNorm << ","
            << remNorm  << ","
            << oscNorm  << ","
            << disNorm  << ","
            << hamNorm
            << "\n";
    }
};
#endif //CHARTBUILDER_H
