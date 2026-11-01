#ifndef TEXT_GENERATOR_HPP
#define TEXT_GENERATOR_HPP

#include <random>
#include <string>

std::string generate_alphabet(std::size_t alphabet_size, std::mt19937& rng);

std::string generate_random(std::size_t length, const std::string& alphabet, std::mt19937& rng);

std::string generate_pattern(const std::string& text, std::size_t length, std::mt19937& rng);

void insert_pattern(std::string& text, const std::string& pattern);

#endif