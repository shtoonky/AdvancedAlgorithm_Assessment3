// build with: g++ -std=c++20 -Iinclude src/benchmark.cpp src/rabin_karp.cpp src/text_generator.cpp -o main.exe

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <random>

#include "rabin_karp.hpp"
#include "text_generator.hpp"

enum class PatternCase{
    Present,
    Absent
};

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
    int pattern_length{20};
    int alphabet_size{26};
    PatternCase pattern_case{PatternCase::Present};
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

TestInput generate_test_input(const Scenario& scenario, std::mt19937& input_rng){
    TestInput input{};

    input.alphabet = generate_alphabet(scenario.alphabet_size, input_rng);
    input.text = generate_random(scenario.text_length, input.alphabet, input_rng);

    if (scenario.experiment == Experiment::PatternRepetition){

        switch (scenario.pattern_structure){
            case PatternStructure::Random:
                input.pattern = "abcdefghijklmnopqrst";
                break;

            case PatternStructure::Alternating:
                input.pattern = "abababababababababab";
                break;

            case PatternStructure::RepeatedCharacter:
                input.pattern = std::string(scenario.pattern_length, 'a');
                break;
        }

        // ensure the pattern is within the text
        if (scenario.pattern_case == PatternCase::Present) {
            input.text.replace(100, input.pattern.size(), input.pattern);
        }

    }
    else {

        switch (scenario.pattern_case){
            case PatternCase::Present:
                input.pattern = generate_pattern(input.text, scenario.pattern_length, input_rng);
                break;
            
            case PatternCase::Absent:
                input.pattern = generate_absent_pattern(input.text, scenario.pattern_length, input.alphabet, input_rng);
                break;
        }        
    }

    return input;
}

RunResult benchmark_rabin_karp(const Scenario& scenario, const TestInput& input, int trial, int q, unsigned int input_seed, unsigned int modulus_seed){
    RabinKarpStats stats{};

    auto rk_start = std::chrono::steady_clock::now();
    
    std::vector<int> rk_result = rabin_karp(input.pattern, input.text, q, &stats);

    auto rk_end = std::chrono::steady_clock::now();
    auto rk_duration = duration_cast<std::chrono::microseconds>(rk_end - rk_start);

    RunResult rabin_karp_record {
    .scenario_name = scenario.name,
    .experiment = scenario.experiment,
    .trial = trial,
    .algorithm = "Rabin-Karp",
    .text_length = static_cast<int>(input.text.size()),
    .pattern_length = static_cast<int>(input.pattern.size()),
    .alphabet_size = scenario.alphabet_size,
    .time_us = rk_duration,
    .matches = static_cast<int>(rk_result.size()),
    .input_seed = input_seed,
    .modulus_seed = modulus_seed,
    .hash_matches = stats.hash_matches,
    .false_positives = stats.false_positives,
    .q = q
    };

    return rabin_karp_record;
}

std::string experiment_to_string(Experiment experiment){
    switch (experiment){
        case Experiment::TextLength:
            return "TextLength";

        case Experiment::PatternLength:
            return "PatternLength";

        case Experiment::AlphabetSize:
            return "AlphabetSize";

        case Experiment::PatternRepetition:
            return "PatternRepetition";

        case Experiment::Modulus:
            return "Modulus";
    }

    throw std::invalid_argument("Unknown experiment");
}

void write_result(std::ofstream& output, const RunResult& result){
    output
        << result.scenario_name << ','
        << experiment_to_string(result.experiment) << ','
        << result.trial << ','
        << result.algorithm << ','
        << result.text_length << ','
        << result.pattern_length << ','
        << result.alphabet_size << ','
        << result.time_us.count() << ','
        << result.matches << ','
        << result.input_seed << ','
        << result.modulus_seed << ','
        << result.hash_matches << ','
        << result.false_positives << ','
        << result.q << '\n';
}

int main(){
    std::ofstream output("results/benchmark_results.csv");

    if (!output){
        std::cerr << "Could not open results file\n";
        return -1;
    }

    output
        << "scenario,experiment,trial,algorithm,"
        << "text_length,pattern_length,alphabet_size,"
        << "time,matches,"
        << "input_seed,modulus_seed,"
        << "hash_matches,false_positives,q\n";

    // Generate generators with fixed seeds for test reproducibility.
    unsigned int input_seed = 12345;
    unsigned int modulus_seed = 67890;
    std::mt19937 input_rng(input_seed);
    std::mt19937 modulus_rng(modulus_seed);

    // Define scenarios for every regime to test.
    std::vector<Scenario> scenarios = {
        // Text lengthg experiment
        {
            .name = "short_text",
            .experiment = Experiment::TextLength,
            .text_length = 10000
        },
        {
            .name = "medium_text",
            .experiment = Experiment::TextLength
        },
        {
            .name = "long_text",
            .experiment = Experiment::TextLength,
            .text_length = 1000000
        },
        // Pattern length experiment
        {
            .name = "short_pattern",
            .experiment = Experiment::PatternLength,
            .pattern_length = 5
        },
        {
            .name = "medium_pattern",
            .experiment = Experiment::PatternLength
        },
        {
            .name = "long_pattern",
            .experiment = Experiment::PatternLength,
            .pattern_length = 50
        },
        // Alphabet size experiment
        {
            .name = "small_alphabet",
            .experiment = Experiment::AlphabetSize,
            .alphabet_size = 5
        },
        {
            .name = "medium_alphabet",
            .experiment = Experiment::AlphabetSize,
            .alphabet_size = 15
        },
        {
            .name = "large_alphabet",
            .experiment = Experiment::AlphabetSize,
        },
        // Pattern repetition experiment
        {
            .name = "low_repetition",
            .experiment = Experiment::PatternRepetition,
        },
        {
            .name = "high_repetition",
            .experiment = Experiment::PatternRepetition,
            .pattern_structure = PatternStructure::Alternating
        },
        {
            .name = "extreme_repetition",
            .experiment = Experiment::PatternRepetition,
            .pattern_structure = PatternStructure::RepeatedCharacter
        }
    };


    for (const Scenario& scenario : scenarios) {

        for (int trial = 0; trial < 30; ++ trial) {
            
            TestInput input = generate_test_input(scenario, input_rng);

            int q = random_prime(10000, 100000, modulus_rng);

            // Benchmark Rabin-Karp
            RunResult rk = benchmark_rabin_karp(scenario, input, trial, q, input_seed, modulus_seed);

            write_result(output, rk);

        }
    }

    // Run modulus experiment
    std::vector<int> moduli = { // Prime values within 10000 and 100000
        19051,
        36479,
        52711,
        58613,
        77201,
        93383
    };

    for (int trial = 0; trial < 30; ++trial){

        std::string alphabet = generate_alphabet(26, input_rng);
        std::string text = generate_random(100000, alphabet, input_rng);
        std::string pattern = generate_pattern(text, 20, input_rng);

        for (int q : moduli){
            RabinKarpStats stats{};

            auto start = std::chrono::steady_clock::now();

            std::vector<int> result = rabin_karp(pattern, text, q, &stats);

            auto end = std::chrono::steady_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            RunResult rk = {
                .scenario_name = "modulus",
                .experiment = Experiment::Modulus,
                .trial = trial,
                .algorithm = "Rabin-Karp",
                .text_length = static_cast<int>(text.size()),
                .pattern_length = static_cast<int>(pattern.size()),
                .alphabet_size = static_cast<int>(alphabet.size()),
                .time_us = duration,
                .matches = static_cast<int>(result.size()),
                .input_seed = input_seed,
                .modulus_seed = modulus_seed,
                .hash_matches = stats.hash_matches,
                .false_positives = stats.false_positives,
                .q = q
            };

            write_result(output, rk);
        }
    }

    return 0;
}