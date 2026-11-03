#include "kmp.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace
{

    void print_vector(const std::vector<int> &values)
    {
        std::cerr << '{';
        for (std::size_t i = 0; i < values.size(); ++i)
        {
            if (i > 0)
                std::cerr << ", ";
            std::cerr << values[i];
        }
        std::cerr << '}';
    }

    void check_equal(const std::vector<int> &actual,
                     const std::vector<int> &expected,
                     const std::string &description)
    {
        if (actual == expected)
            return;

        std::cerr << "FAIL: " << description << "\nExpected: ";
        print_vector(expected);
        std::cerr << "\nActual:   ";
        print_vector(actual);
        std::cerr << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::vector<int> naive_search(const std::string &pattern,
                                  const std::string &text)
    {
        std::vector<int> matches;
        if (pattern.empty() || pattern.size() > text.size())
            return matches;

        for (std::size_t i = 0; i <= text.size() - pattern.size(); ++i)
        {
            if (text.compare(i, pattern.size(), pattern) == 0)
            {
                matches.push_back(static_cast<int>(i));
            }
        }
        return matches;
    }

    std::vector<int> naive_lps(const std::string &pattern)
    {
        std::vector<int> result(pattern.size(), 0);
        for (std::size_t i = 1; i < pattern.size(); ++i)
        {
            for (std::size_t length = i; length > 0; --length)
            {
                if (pattern.compare(0, length, pattern,
                                    i + 1 - length, length) == 0)
                {
                    result[i] = static_cast<int>(length);
                    break;
                }
            }
        }
        return result;
    }

    struct SearchCase
    {
        std::string name;
        std::string pattern;
        std::string text;
        std::vector<int> expected;
    };

} // namespace

int main()
{
    const std::vector<std::pair<std::string, std::vector<int>>> lps_cases = {
        {"ababaca", {0, 0, 1, 2, 3, 0, 1}},
        {"aaaa", {0, 1, 2, 3}},
        {"abcde", {0, 0, 0, 0, 0}},
        {"ababab", {0, 0, 1, 2, 3, 4}},
        {"a", {0}},
        {"", {}}};
    for (const auto &test : lps_cases)
    {
        check_equal(build_lps(test.first), test.second,
                    "LPS pattern='" + test.first + "'");
    }

    const std::vector<SearchCase> search_cases = {
        {"normal matches", "abra", "abracadabra", {0, 7}},
        {"repeated overlapping matches", "aaa", "aaaaa", {0, 1, 2}},
        {"prefix-suffix overlaps", "aba", "ababa", {0, 2}},
        {"no match", "xyz", "abcde", {}},
        {"pattern equals text", "abc", "abc", {0}},
        {"pattern longer than text", "abcd", "abc", {}},
        {"empty pattern", "", "abc", {}},
        {"empty text", "a", "", {}},
        {"both empty", "", "", {}},
        {"banana overlaps", "ana", "banana", {1, 3}},
        {"case sensitive", "Aa", "aAaA", {1}},
        {"match at end", "ab", "xxab", {2}},
        {"single character", "a", "banana", {1, 3, 5}},
        {"long fallback", "ababaca", "ababababaca", {4}},
        {"non-overlapping matches", "abc", "abcabc", {0, 3}},
        {"no near match", "aaab", "aaaaaaaaa", {}},
        {"punctuation", "a.a", "a.a.a", {0, 2}},
        {"alternating pattern", "ab", "abab", {0, 2}}};
    for (const auto &test : search_cases)
    {
        check_equal(kmp_search(test.pattern, test.text), test.expected,
                    test.name);
    }

    std::mt19937 rng(41052);
    std::uniform_int_distribution<int> text_length(0, 120);
    std::uniform_int_distribution<int> pattern_length(0, 30);
    const int alphabets[] = {1, 2, 3, 26};
    constexpr int random_trials = 5000;

    for (int trial = 0; trial < random_trials; ++trial)
    {
        const int alphabet_size = alphabets[trial % 4];
        std::uniform_int_distribution<int> character(0, alphabet_size - 1);

        auto random_string = [&](int length)
        {
            std::string result;
            result.reserve(static_cast<std::size_t>(length));
            for (int i = 0; i < length; ++i)
            {
                result.push_back(static_cast<char>('a' + character(rng)));
            }
            return result;
        };

        const std::string text = random_string(text_length(rng));
        std::string pattern = random_string(pattern_length(rng));

        if (trial % 3 == 0 && !text.empty())
        {
            std::uniform_int_distribution<int> start_pos(
                0, static_cast<int>(text.size()) - 1);
            const int start = start_pos(rng);
            std::uniform_int_distribution<int> match_length(
                1, std::min(30, static_cast<int>(text.size()) - start));
            pattern = text.substr(static_cast<std::size_t>(start),
                                  static_cast<std::size_t>(match_length(rng)));
        }

        check_equal(kmp_search(pattern, text), naive_search(pattern, text),
                    "random search trial " + std::to_string(trial));
        check_equal(build_lps(pattern), naive_lps(pattern),
                    "random LPS trial " + std::to_string(trial));
    }

    std::cout << "PASS: " << lps_cases.size() << " LPS examples, "
              << search_cases.size() << " search edge cases, and "
              << random_trials << " random search/LPS cross-checks\n";
    return 0;
}