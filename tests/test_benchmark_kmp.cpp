#include "benchmarp.hpp"
#include "kmp.hpp"

#include <iostream>
#include <string>

namespace
{
    bool check(bool condition, const std::string &message)
    {
        if (!condition)
        {
            std::cerr << "FAIL: " << message << '\n';
            return false;
        }
        return true;
    }
}

int main()
{
    Scenario scenario{
        .name = "kmp_smoke_test",
        .experiment = Experiment::PatternRepetition,
        .text_length = 5,
        .pattern_length = 3,
        .alphabet_size = 1,
        .pattern_structure = PatternStructure::RepeatedCharacter};

    const TestInput input{.text = "aaaaa", .pattern = "aaa", .alphabet = "a"};
    const RunResult result = benchmark_kmp(scenario, input, 2, 12345, 67890);

    if (!check(result.algorithm == "KMP", "algorithm label"))
        return 1;
    if (!check(result.scenario_name == scenario.name, "scenario name"))
        return 1;
    if (!check(result.experiment == scenario.experiment, "experiment"))
        return 1;
    if (!check(result.trial == 2, "trial"))
        return 1;
    if (!check(result.text_length == 5 && result.pattern_length == 3, "input lengths"))
        return 1;
    if (!check(result.matches == 3, "overlapping match count"))
        return 1;
    if (!check(result.pattern_structure == scenario.pattern_structure, "pattern structure"))
        return 1;
    if (!check(result.input_seed == 12345 && result.modulus_seed == 67890, "seeds"))
        return 1;
    if (!check(result.hash_matches == -1 && result.false_positives == -1 && result.q == -1,
               "KMP has no hash statistics"))
        return 1;
    if (!check(result.time_us.count() >= 0, "non-negative duration"))
        return 1;

    const TestInput no_match{.text = "abcde", .pattern = "xyz", .alphabet = "abcde"};
    if (!check(benchmark_kmp(scenario, no_match, 3, 12345, 67890).matches == 0,
               "no-match count"))
        return 1;

    std::cout << "PASS: KMP benchmark integration smoke tests\n";
    return 0;
}
