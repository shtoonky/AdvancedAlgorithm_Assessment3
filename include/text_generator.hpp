#ifndef TEXT_GENERATOR_HPP
#define TEXT_GENERATOR_HPP

#include <stdexcept>
#include <random>
#include <string>

std::string generate_alphabet(std::size_t alphabet_size, std::mt19937& rng);

std::string generate_random(std::size_t length, const std::string& alphabet, std::mt19937& rng);

std::string generate_pattern(const std::string& text, std::size_t length, std::mt19937& rng);

std::string generate_absent_pattern(const std::string& text, std::size_t length, const std::string& alphabet, std::mt19937& rng);

// std::string generate_repeated_patterns(const std::string& text, std::size_t pattern_length, std::size_t min_occurrences, std::mt19937& rng);

#endif