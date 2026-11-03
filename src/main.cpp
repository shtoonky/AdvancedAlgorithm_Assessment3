// build with: g++ -std=c++20 -Iinclude src/benchmark.cpp src/rabin_karp.cpp src/kmp.cpp src/text_generator.cpp src/main.cpp -o main.exe

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <random>

#include "benchmarp.hpp"
#include "text_generator.hpp"
#include "rabin_karp.hpp"
#include "kmp.hpp"

int main(){
    std::ofstream output("results/benchmark_results_raw.csv");

    if (!output){
        std::cerr << "Could not open results file\n";
        return -1;
    }

    output
        << "scenario,experiment,trial,algorithm,"
        << "text_length,pattern_length,alphabet_size,"
        << "time_us,matches,pattern_structure,"
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
            .experiment = Experiment::PatternLength,
            .pattern_length = 50
        },
        {
            .name = "long_pattern",
            .experiment = Experiment::PatternLength,
            .pattern_length = 200
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

    const static int q = random_prime(10000, 100000, modulus_rng);

    // Warm Up (not timed.)
    Scenario warm_up_scenario {
        .name = "warm_up",
        .experiment = Experiment::PatternLength
    };
    TestInput input = generate_test_input(warm_up_scenario, input_rng);

    rabin_karp(input.pattern, input.text, q); // Run KMP too
    
    kmp_search(input.pattern, input.text);

    // Standard Experiments

    for (const Scenario& scenario : scenarios) {

        for (int trial = 0; trial < 30; ++ trial) {
            
            TestInput input = generate_test_input(scenario, input_rng);

            // Benchmark Rabin-Karp
            RunResult rk = benchmark_rabin_karp(scenario, input, trial, q, input_seed, modulus_seed);
            RunResult kmp = benchmark_kmp(scenario, input, trial, input_seed, modulus_seed);

            // Check they're the same
            std::vector<int> rk_matches = rabin_karp(input.pattern, input.text, q);
            std::vector<int> kmp_matches = kmp_search(input.pattern, input.text);

            if (rk_matches != kmp_matches) {
                std::cerr << "Algorithms returned different results\n";
                std::cerr << "Scenario: " << scenario.name << '\n';
                std::cerr << "Trial: " << trial << '\n';
                return -1;
            }

            // Write results to CSV
            write_result(output, rk);
            write_result(output, kmp);
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
        std::string pattern = generate_pattern(text, 50, input_rng);

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