/*
Few methods to generate random, small alphabet texts and a pattern from a text.
Natural language text can be generated directly.
*/
#include <algorithm>
#include <fstream>
#include <sstream>
// #include <iostream>

#include "text_generator.hpp"

// unsigned int input_seed = 12345;
// static mt19937 gen(input_seed);

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

    std::uniform_int_distribution<std::size_t> distrib(0, text.size() - length);

    std::size_t start = distrib(rng);
    return text.substr(start, length);
}

// Returns a string of size length comprised of letter from alphabet and is NOT present in the given text.
std::string generate_absent_pattern(const std::string& text, std::size_t length, const std::string& alphabet, std::mt19937& rng){
    while (true){
        std::string pattern = generate_random(length, alphabet, rng);

        if (text.find(pattern) == std::string::npos){
            return pattern;
        }
    }
}

// Returns a string of text from a file.
std::string load_natural_text(const std::string& filename){
    std::ifstream input(filename);

    if (!input)
        throw std::runtime_error("Could not open text file " + filename);

    std::ostringstream buffer;
    buffer << input.rdbuf();

    std::string text = buffer.str();

    if (text.empty()) {
        throw std::runtime_error("Text file is empty: " + filename);
    }

    return text;
}

// Potentially find a repeated pattern within a text of size pattern_length which can be found in the text at least min_occurences times.
std::string generate_repeated_patterns(const std::string& text, std::size_t pattern_length, std::size_t min_occurrences, std::mt19937& rng) {
    if (pattern_length == 0 || pattern_length > text.size())
        throw std::invalid_argument("Invalid pattern length");
    
    if (min_occurrences == 0)
        throw std::invalid_argument("Minimum occurences must be possible");

    std::uniform_int_distribution<size_t> dist(0, text.size() - pattern_length);

    constexpr std::size_t max_attempts = 10000;

    for (std::size_t attempt = 0; attempt < max_attempts; ++attempt){
        std::size_t start = dist(rng);
        std::string pattern = text.substr(start, pattern_length);

        size_t count = 0;
        size_t pos = 0;

        while ((pos = text.find(pattern, pos)) != std::string::npos) {
            ++count;
            ++pos;
        }

        if (count >= min_occurrences) {
            return pattern;
        }
    }

    throw std::runtime_error(
        "Could not find a pattern with the required number of occurrences"
    );

    return "";
}
