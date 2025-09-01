cmake_minimum_required(VERSION 3.31)
project(DisorderMetrics)

set(CMAKE_CXX_STANDARD 20)

include_directories("C:/Users/markg/Downloads/matplotplusplus-master/matplotplusplus-master/source")

add_executable(DisorderMetrics main.cpp
        src/Analysis/DisorderMetrics.cpp
        src/Analysis/DisorderMetrics.h
        src/Data/DataGenerator.cpp
        src/Data/DataGenerator.h
        src/Algotrithms/QuickSort.cpp
        src/Algotrithms/QuickSort.h
        src/Algotrithms/InsertionSort.cpp
        src/Algotrithms/InsertionSort.h
        src/Algotrithms/SortingAlgorithm.h
        src/Algotrithms/MergeSort.cpp
        src/Algotrithms/MergeSort.h
        src/Algotrithms/BinaryInsertionSort.cpp
        src/Algotrithms/BinaryInsertionSort.h
        src/Algotrithms/AdaptiveShiversSort.cpp
        src/Algotrithms/AdaptiveShiversSort.h
        src/Algotrithms/PowerSort.h
        src/Algotrithms/TimSort.h
        src/AdaptiveAlg.h
        src/Data/Sampler.h
        src/Data/SamplingStrategy.h
        src/Analysis/Evaluator.h
        src/Analysis/Metric.h
        src/Data/ArrayType.h
        src/Data/ExperimentConfigurator.h)
