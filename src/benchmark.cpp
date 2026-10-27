// build with: g++ -std=c++20 -Iinclude src/benchmark.cpp src/rabin_karp.cpp src/text_generator.cpp -o main.exe

#include <chrono>
#include <fstream>
#include <iostream>

#include "rabin_karp.hpp"
#include "text_generator.hpp"

enum class TextSource {
    Random,
    RandomRestrictedAlphabet,
    Natural
};

enum class PatternCase{
    Present,
    Absent,
    Repeated
};

struct Scenario{
    std::string name;
    TextSource source;
    int text_length;
    int pattern_length;
    int alphabet_size{26};
    PatternCase pattern_case;
    int min_occurrence{2};
};

struct RunResult{
    std::string scenario_name;
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

void write_result(std::ofstream& output, const RunResult& result){
    output
        << result.scenario_name << ','
        << result.trial << ','
        << result.algorithm << ','
        << result.text_length << ','
        << result.pattern_length << ','
        << result.alphabet_size << ','
        << result.time_us << ','
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
        << "scenario,trial,algorithm,"
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
        {"short_text", TextSource::Random, 10000, 20, 26, PatternCase::Absent},
        {"medium_text", TextSource::Random, 100000, 20, 26, PatternCase::Absent},
        {"long_text", TextSource::Random, 1000000, 20, 26, PatternCase::Absent},
        {"short_text", TextSource::Random, 10000, 20, 26, PatternCase::Present},
        {"medium_text", TextSource::Random, 100000, 20, 26, PatternCase::Present},
        {"long_text", TextSource::Random, 1000000, 20, 26, PatternCase::Present},
        {"short_natural", TextSource::Natural, 200, 2, 26, PatternCase::Repeated}
    };

    // Load natural text once.
    const std::string natural_text = load_natural_text("data/natural_text.txt");

    for (int trial = 0; trial < 3; trial++){
        int q = random_prime(10000, 100000, modulus_rng);

        for (const Scenario& scenario : scenarios){

            // Generate text and pattern
            std::string alphabet = generate_alphabet(scenario.alphabet_size, input_rng);;
            std::string text;

            switch (scenario.source){
                case TextSource::Random:
                case TextSource::RandomRestrictedAlphabet:
                    text = generate_random(scenario.text_length, alphabet, input_rng);
                    break;

                case TextSource::Natural:
                    if (natural_text.size() < static_cast<size_t>(scenario.text_length)){ 
                        throw std::runtime_error("Natural text is shorter than requested text length");
                    }

                    text = natural_text.substr(0, scenario.text_length);
                    break;

                default:
                    throw std::invalid_argument("Incorrect text source at trial: " + trial);
                    break;
            }

            std::string pattern;

            switch (scenario.pattern_case){
                case PatternCase::Present:
                    pattern = generate_pattern(text, scenario.pattern_length, input_rng);
                    break;
                
                case PatternCase::Absent:
                    pattern = generate_absent_pattern(text, scenario.pattern_length, alphabet, input_rng);
                    break;

                case PatternCase::Repeated:
                    pattern = generate_repeated_patterns(text, scenario.pattern_length, scenario.min_occurrence, input_rng);
                    break;

                default:
                    throw std::invalid_argument("Incorrect pattern case at trial: " + trial);
                    break;   
            }

            // Run and time Rabin-Karp algorithm.
            auto start = std::chrono::steady_clock::now();
            
            RabinKarpStats stats;

            std::vector<int> rabin_karp_result = rabin_karp(pattern, text, q, &stats);

            auto end = std::chrono::steady_clock::now();
            auto rabin_karp_duration = duration_cast<std::chrono::microseconds>(end - start);

            // Run and time KMP algorithm.

            // Record results.

            RunResult rabin_karp_record {
                .scenario_name = scenario.name,
                .trial = trial,
                .algorithm = "Rabin-Karp",
                .text_length = scenario.text_length,
                .pattern_length = scenario.pattern_length,
                .alphabet_size = scenario.alphabet_size,
                .time_us = rabin_karp_duration,
                .matches = (int)rabin_karp_result.size(),
                .input_seed = input_seed,
                .modulus_seed = modulus_seed,
                .hash_matches = stats.hash_matches,
                .false_positives = stats.false_positives,
                .q = stats.q
            };

            write_result(output, rabin_karp_record);
        }
    }

    return 0;
}