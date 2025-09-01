#include <iostream>


#include "src/AdaptiveAlg.h"
#include "src/Algotrithms/AdaptiveShiversSort.h"
#include "src/Algotrithms/MergeSort.h"
#include "src/Algotrithms/PowerSort.h"
#include "src/Analysis/Evaluator.h"
#include "src/Data/DataGenerator.h"
#include "src/Data/Sampler.h"
#include "src/Analysis/DisorderMetrics.h"
#include "src/Data/ExperimentConfigurator.h"

namespace fs = std::filesystem;
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {


    // ExperimentConfigurator configurator(
    //        10000, 100, 0, 10000,
    //        "C:/Users/markg/CLionProjects/DisorderMetrics/src/experiment_data_input"
    //    );
    //
    // std::vector<int> kValues   = {2500, 5000, 7500, 9000};
    // std::vector<int> runsValues = {
    //     13, 100, 200, 1000, 2500,
    //     (int)std::sqrt(10000),
    //     2*(int)std::sqrt(10000),
    //     (int)std::log2(10000),
    //     10000/4, 10000/10, 10000/20
    // };
    //
    // configurator.configure(1000, kValues, runsValues);

    Evaluator ev;
    std::string in  = "C:/Users/markg/CLionProjects/DisorderMetrics/src/experiment_data_input";
    std::string out = "C:/Users/markg/CLionProjects/DisorderMetrics/src/experiment_data_output";

    ev.prepareOutputStructure(in, out);
    ev.evaluateAll(in, out);


}



