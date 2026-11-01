

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <random>

#include "rabin_karp.hpp"
#include "text_generator.hpp"
#include "benchmarp.hpp"

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
        insert_pattern(input.text, input.pattern);

    }
    else {
        input.pattern = generate_pattern(input.text, scenario.pattern_length, input_rng);
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
    .pattern_structure = scenario.pattern_structure,
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

std::string pattern_struct_to_string(PatternStructure pattern_structure) {
    switch (pattern_structure){
        case PatternStructure::Random:
            return "Random";

        case PatternStructure::Alternating:
            return "Alternating";

        case PatternStructure::RepeatedCharacter:
            return "RepeatedCharacter";
    }

    throw std::invalid_argument("Unknown pattern structure");
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
        << pattern_struct_to_string(result.pattern_structure) << ','
        << result.input_seed << ','
        << result.modulus_seed << ','
        << result.hash_matches << ','
        << result.false_positives << ','
        << result.q << '\n';
}
