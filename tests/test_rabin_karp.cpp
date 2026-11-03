// g++ -std=c++20 -O2 -Iinclude tests/test_rabin_karp.cpp src/rabin_karp.cpp -o test_rabin_karp.exe

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "rabin_karp.hpp"

using namespace std;

// Simple independent search implementation for comparison.
vector<int> naive_search(const string& pattern, const string& text) {
    vector<int> matches;

    if (pattern.empty() || pattern.size() > text.size()) {
        return {};
    }

    for (size_t i = 0; i <= text.size() - pattern.size(); ++i) {
        if (text.compare(i, pattern.size(), pattern) == 0) {
            matches.push_back(static_cast<int>(i));
        }
    }

    return matches;
}

void test_basic_match() {
    string text = "abcdefabc";
    string pattern = "abc";

    vector<int> expected = {0, 6};

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_overlapping_matches() {
    string text = "aaaaa";
    string pattern = "aaa";

    vector<int> expected = {0, 1, 2};

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_no_match() {
    string text = "abcdefgh";
    string pattern = "xyz";

    vector<int> expected;

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_pattern_equals_text() {
    string text = "algorithm";
    string pattern = "algorithm";

    vector<int> expected = {0};

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_pattern_longer_than_text() {
    string text = "abc";
    string pattern = "abcdef";

    vector<int> expected = {};

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_empty_pattern() {
    string text = "abcdef";
    string pattern = "";

    vector<int> expected = {};

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_match_at_end() {
    string text = "abcdefgh";
    string pattern = "fgh";

    vector<int> expected = {5};

    assert(rabin_karp(pattern, text, 101) == expected);
}

void test_hash_collision() {
    string text = "c";
    string pattern = "a";

    RabinKarpStats stats;

    vector<int> matches = rabin_karp(pattern, text, 2, &stats);

    cout << "Matches: " << matches.size() << '\n';
    cout << "Hash matches: " << stats.hash_matches << '\n';
    cout << "False positives: " << stats.false_positives << '\n';

    assert(matches.empty());
    assert(stats.hash_matches == 1);
    assert(stats.false_positives == 1);
}

void test_statistics() {
    string text = "aaaaa";
    string pattern = "aaa";

    RabinKarpStats stats;

    vector<int> matches = rabin_karp(pattern, text, 101, &stats);

    assert(matches == vector<int>({0, 1, 2}));
    assert(stats.hash_matches == 3);
    assert(stats.false_positives == 0);
}

void test_randomised_against_naive() {
    mt19937 rng(41052);

    uniform_int_distribution<int> text_length_dist(1, 120);
    uniform_int_distribution<int> pattern_length_dist(1, 30);
    uniform_int_distribution<int> character_dist(0, 3);

    const string alphabet = "abcd";

    constexpr int trials = 5000;
    constexpr int q = 101;

    for (int trial = 0; trial < trials; ++trial) {
        int text_length = text_length_dist(rng);
        int pattern_length = pattern_length_dist(rng);

        string text;
        string pattern;

        for (int i = 0; i < text_length; ++i) {
            text += alphabet[character_dist(rng)];
        }

        for (int i = 0; i < pattern_length; ++i) {
            pattern += alphabet[character_dist(rng)];
        }

        vector<int> expected = naive_search(pattern, text);
        vector<int> actual = rabin_karp(pattern, text, q);

        if (actual != expected) {
            cerr << "Randomised test failed!\n";
            cerr << "Trial: " << trial << '\n';
            cerr << "Text: " << text << '\n';
            cerr << "Pattern: " << pattern << '\n';

            cerr << "Expected: ";
            for (int pos : expected) cout << pos << " ";
            cout << '\n';

            cerr << "Actual: ";
            for (int pos : actual) cout << pos << " ";
            cout << '\n';

            assert(actual == expected);
        }
    }
}

int main() {
    test_basic_match();
    test_overlapping_matches();
    test_no_match();
    test_pattern_equals_text();
    test_pattern_longer_than_text();
    test_empty_pattern();
    test_match_at_end();
    test_hash_collision();
    test_statistics();
    test_randomised_against_naive();

    cout << "All Rabin-Karp tests passed!\n";

    return 0;
}