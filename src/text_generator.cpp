/*
Few methods to generate random, small alphabet texts and a pattern from a text.
Natural language text can be generated directly.
*/

#include <iostream>
#include <random>
#include <string>

using namespace std;

static random_device rd;
static mt19937 gen(rd());

// Generates a string of size length comprised of random lowercase characters.
string generate_random(size_t length) {

    if (length == 0){
        throw invalid_argument("Invalid text length");
    }

    uniform_int_distribution<int> distrib(0, 25);
        
    string output;
    output.reserve(length);

    for (size_t i = 0; i < length; ++i){
        char random_char = static_cast<char>('a' + distrib(gen));
        output += random_char;
    }

    return output;
}

// Generates a string of size length comprised on alphabet_size distinct lowercase characters.
string generate_random_small_alphabet(size_t length, size_t alphabet_size) {
    uniform_int_distribution<int> distrib(0, 25);

    if (alphabet_size > 26 || alphabet_size == 0 || length == 0){
        throw invalid_argument("Invalid alphabet size or text length");
    }

    std::vector<char> random_chars;

    while (random_chars.size() < alphabet_size){
        char random_char = static_cast<char>('a' + distrib(gen));

        if (find(random_chars.begin(), random_chars.end(), random_char) != random_chars.end()){
            continue;
        }
        random_chars.push_back(random_char);
    }
        
    string output;
    output.reserve(length);

    uniform_int_distribution<int> alphabet_distrib(0, alphabet_size - 1);

    for (size_t i = 0; i < length; ++i){
        char random_char = random_chars[alphabet_distrib(gen)];
        output += random_char;
    }

    return output;
}

// Returns a substring of text of size length at a random index.
string generate_pattern(const string& text, size_t length){
    if (length == 0 || length > text.size()){
        throw invalid_argument("Invalid pattern length");
    }

    static random_device rd;
    static mt19937 gen(rd());

    uniform_int_distribution<size_t> distrib(0, text.size() - length);

    size_t start = distrib(gen);
    return text.substr(start, length);
}
