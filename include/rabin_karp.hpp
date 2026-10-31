#ifndef RABIN_KARP_HPP
#define RABIN_KARP_HPP

#include <vector>
#include <string>
#include <random>

// Record for output.
struct RabinKarpStats{
    int hash_matches = 0;
    int false_positives = 0;
};

std::vector<int> rabin_karp(const std::string& pattern, const std::string& text, int q, RabinKarpStats* stats = nullptr);

int random_prime(int min_prime, int max_prime, std::mt19937& modulus_rng);

#endif