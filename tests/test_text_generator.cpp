// g++ -std=c++20 -Iinclude tests/test_text_generator.cpp src/text_generator.cpp -o test_text_generator.exe

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "text_generator.hpp"

// Counts occurrences, including overlapping occurrences
size_t count_occurrences(const std::string& text, const std::string& pattern) {
    size_t count = 0;
    size_t pos = text.find(pattern);

    while (pos != std::string::npos) {
        ++count;
        pos = text.find(pattern, pos + 1);
    }

    return count;
}

void test_pattern_occurs_exactly_once() {
    std::string text = "abcdefghij";
    size_t length = 3;

    std::mt19937 rng(12345);

    std::string pattern = generate_pattern(text, length, rng);

    assert(pattern.size() == length);
    assert(count_occurrences(text, pattern) == 1);
}

void test_overlapping_occurrences_are_detected() {
    std::string text = "aaaaab";
    std::size_t length = 2;

    std::mt19937 rng(12345);

    std::string pattern = generate_pattern(text, length, rng);

    // "aa" occurs multiple times, including overlapping occurrences
    // The only valid pattern is "ab"
    assert(pattern == "ab");
    assert(count_occurrences(text, pattern) == 1);
}

void test_invalid_pattern_length() {
    std::mt19937 rng(12345);

    // Zero-length pattern
    bool threw = false;

    try {
        generate_pattern("abcdef", 0, rng);
    }
    catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);

    // Pattern longer than text
    threw = false;

    try {
        generate_pattern("abc", 4, rng);
    }
    catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
}

void test_no_unique_pattern_throws() {
    std::string text = "aaaaaaaa";
    size_t length = 3;

    std::mt19937 rng(12345);

    bool threw = false;

    try {
        generate_pattern(text, length, rng);
    }
    catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
}

int main() {
    test_pattern_occurs_exactly_once();
    test_overlapping_occurrences_are_detected();
    test_invalid_pattern_length();
    test_no_unique_pattern_throws();

    std::cout << "All text generator tests passed!\n";

    return 0;
}