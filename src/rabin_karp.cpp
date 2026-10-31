/*
Rabin-Karp Algorithm
Hannah Effat Manesh Nik
25809221
*/
#include <stdexcept>
#include <algorithm>


#include "rabin_karp.hpp"

std::vector<int> rabin_karp(const std::string &pattern, const std::string &text, int q, RabinKarpStats* stats){

    if (pattern.empty() || pattern.length() > text.length())
        return {};

    if (q <= 1) 
        throw std::invalid_argument("Modulus must be greater than 1.");


    if (stats){    
        stats->hash_matches = 0;
        stats->false_positives = 0;
    }

    int d = 256; // No. of chars in input alphabet - assumed ASCII

    int m = pattern.length();
    int n = text.length();

    int p = 0; // Numerical hash value for pattern
    int t = 0; // Numerical hash value for current window of text

    int h = 1; // Highest position multiplier -> stores d^(m-1) to let the algo remove outgoing characters

    std::vector<int> ans;

    // Precompute h 
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    // Compute initial hash values for pattern and first window of text
    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    // Slide pattern over text by one, finishing at the last possible starting position
    for (int i = 0; i <= n - m; i++){
        
        // If hash values match, check characters
        if (p == t){
            if (stats)
                stats->hash_matches++;

            bool match = true;
            
            for (int j = 0; j < m; j ++){ // We must manually check in case of a hash collision
                if (text[i + j] != pattern[j]){
                    match = false;
                    break;
                }
            }

            if (match){
                ans.push_back(i);
            }
            else {
                if (stats)
                    stats->false_positives++;
            }
        }

        if (i < n - m){ // Calculate the hash of the next windows without hashing all characters again
            long long next_hash = (static_cast<long long>(d) * (t - static_cast<long long>(text[i] * h)) + text[i + m]) % q;

            if (next_hash < 0) // Basically converts a negative hash to the positive equivalent
                next_hash += q;

            t = static_cast<int>(next_hash);
        }
    }

    return ans;
}

// Functions to generate and select a random prime number within a range 
// to use as the modulus for hashing

std::vector<int> generate_primes(int limit){
    std::vector<bool> is_prime(limit + 1, true);

    is_prime[0] = is_prime[1] = false;

    for (int i = 2; i * i <= limit; i++){
        if (is_prime[i]){
            for (int j = i * i; j <= limit; j += i){
                is_prime[j] = false;
            }
        }
    }

    std::vector<int> primes;

    for (int i = 2; i <= limit; i++){
        if (is_prime[i]){
            primes.push_back(i);
        }
    }

    return primes;
}

const std::vector<int>& get_primes() {
    static const std::vector<int> primes = generate_primes(100000); // Only get primes in range once
    return primes;
}

int random_prime(int min_prime, int max_prime, std::mt19937& rng){

    const std::vector<int>& primes = get_primes();

    auto first = std::lower_bound(primes.begin(), primes.end(), min_prime); // find the first prime >= to min_prime
    auto last = std::upper_bound(primes.begin(), primes.end(), max_prime); // find the first prime strictly > than max_prime

    if (first == last){
        throw std::invalid_argument("No primes in specified range.");
    }

    std::uniform_int_distribution<int> dist(0, static_cast<int>(last - first) - 1);

    return *(first + dist(rng));
}