cmake_minimum_required(VERSION 3.20)
project(DisorderMetrics CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- внешний include (если нужен) ---
include_directories("C:/Users/markg/Downloads/matplotplusplus-master/matplotplusplus-master/source")

# ================== Приложение ==================
add_executable(DisorderMetrics
        "C:/Users/markg/CLionProjects/DisorderMetrics/main.cpp"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/DisorderMetrics.cpp"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/DisorderMetrics.h"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/Evaluator.h"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/Metric.h"
)
target_include_directories(DisorderMetrics PRIVATE
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Data"
)

# ================== GoogleTest ==================
include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# Подстраховка include-путей GTest (source/binary/_deps)
set(GTEST_INC_CANDIDATES
        "${googletest_SOURCE_DIR}/googletest/include"
        "${googletest_SOURCE_DIR}/googlemock/include"
        "${googletest_BINARY_DIR}/googletest/include"
        "${googletest_BINARY_DIR}/googlemock/include"
        "C:/Users/markg/CLionProjects/DisorderMetrics/cmake-build-debug/_deps/googletest-src/googletest/include"
        "C:/Users/markg/CLionProjects/DisorderMetrics/cmake-build-debug/_deps/googletest-src/googlemock/include"
        "C:/Users/markg/CLionProjects/DisorderMetrics/cmake-build-debug/_deps/googletest-build/googletest/include"
        "C:/Users/markg/CLionProjects/DisorderMetrics/cmake-build-debug/_deps/googletest-build/googlemock/include"
)

enable_testing()
include(GoogleTest)

# ================== Тесты ==================
add_executable(DisorderMetricsTest
        "C:/Users/markg/CLionProjects/DisorderMetrics/tests/test_runner.cpp"
        # заголовки с тестами — для IDE (код подключается через #include в test_runner.cpp)
        "C:/Users/markg/CLionProjects/DisorderMetrics/tests/DisorderMetricsTest.h"
        "C:/Users/markg/CLionProjects/DisorderMetrics/tests/SamplerTest.h"
        "C:/Users/markg/CLionProjects/DisorderMetrics/tests/DataGeneratorTest.h"
        # исходники, используемые тестами
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/DisorderMetrics.cpp"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis/DisorderMetrics.h"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Data/DataGenerator.cpp"
)
target_include_directories(DisorderMetricsTest PRIVATE
        "C:/Users/markg/CLionProjects/DisorderMetrics"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Analysis"
        "C:/Users/markg/CLionProjects/DisorderMetrics/src/Data"
        ${GTEST_INC_CANDIDATES}
)
target_link_libraries(DisorderMetricsTest PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Регистрация тестов
gtest_discover_tests(DisorderMetricsTest)