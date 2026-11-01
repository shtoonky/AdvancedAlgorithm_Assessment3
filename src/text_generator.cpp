/*
Few methods to generate random, small alphabet texts and a pattern from a text.
*/
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "text_generator.hpp"

// Generates an string of distinct letters from the lowercase alphabet of size alphabet_size.
std::string generate_alphabet(std::size_t alphabet_size, std::mt19937& rng){
    if (alphabet_size == 0 || alphabet_size > 26){
        throw std::invalid_argument("Invalid alphabet size");
    }
    
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    shuffle(alphabet.begin(), alphabet.end(), rng);

    alphabet.resize(alphabet_size);

    return alphabet;
}

// Generates a string of size length comprised of letters from a given alphabet.
std::string generate_random(std::size_t length, const std::string& alphabet, std::mt19937& rng){
    if (length == 0 || alphabet.empty()){
        throw std::invalid_argument("Invalid length or alphabet");
    }

    std::uniform_int_distribution<size_t> distrib(0, alphabet.size() - 1);

    std::string output;
    output.reserve(length);

    for (size_t i = 0; i < length; ++i){
        output += alphabet[distrib(rng)];
    }

    return output;
}

// Returns a substring of text of size length at a random index.
std::string generate_pattern(const std::string& text, std::size_t length, std::mt19937& rng){
    if (length == 0 || length > text.size()){
        throw std::invalid_argument("Invalid pattern length");
    }

    constexpr std::size_t max_attempts = 10000;

    std::uniform_int_distribution<std::size_t> distrib(0, text.size() - length);

    for (std::size_t attempt = 0; attempt < max_attempts; ++attempt){

        std::size_t start = distrib(rng);

        size_t occurrences = text.find(start);

        if (occurrences == std::string::npos || occurrences > 1){
            continue;
        }

        return text.substr(start, length);
    }

    throw std::runtime_error("Could not generate an single occurence pattern");
}

void insert_pattern(std::string& text, const std::string& pattern) {
    if (text.find(pattern) == std::string::npos) {
        text.replace(100, pattern.size(), pattern);
    }
}
