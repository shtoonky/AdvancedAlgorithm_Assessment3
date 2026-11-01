#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <string>
#include <chrono>

enum class Experiment {
    TextLength,
    PatternLength,
    AlphabetSize,
    PatternRepetition,
    Modulus
};

enum class PatternStructure {
    Random,
    Alternating,
    RepeatedCharacter
};

struct Scenario{
    std::string name;
    Experiment experiment;

    int text_length{100000};
    int pattern_length{50};
    int alphabet_size{26};
    PatternStructure pattern_structure{PatternStructure::Random};
};

struct RunResult{
    std::string scenario_name;
    Experiment experiment;

    int trial;
    std::string algorithm;

    int text_length;
    int pattern_length;
    int alphabet_size;

    std::chrono::microseconds time_us;
    int matches;

    PatternStructure pattern_structure;

    unsigned int input_seed;
    unsigned int modulus_seed;

    int hash_matches{-1};
    int false_positives{-1};
    int q{-1};
};

struct TestInput {
    std::string text;
    std::string pattern;
    std::string alphabet;
};

TestInput generate_test_input(const Scenario& scenario, std::mt19937& input_rng);

RunResult benchmark_rabin_karp(const Scenario& scenario, const TestInput& input, int trial, int q, unsigned int input_seed, unsigned int modulus_seed);

void write_result(std::ofstream& output, const RunResult& result);

#endif