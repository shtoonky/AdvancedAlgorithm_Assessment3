/*
Rabin-Karp Algorithm
Hannah Effat Manesh Nik
25809221
*/
#include "include/rabin_karp.hpp"

vector<int> rabin_karp(const string &pattern, const string &text, int q){

    if (pattern.empty() || pattern.length() > text.length())
        return {-1};

    int d = 256; // No. of chars in input alphabet - assumed ASCII

    int m = pattern.length();
    int n = text.length();

    int p = 0; // Numerical hash value for pattern
    int t = 0; // Numerical hash value for current window of text

    int h = 1; // Highest position multiplier -> stores d^(m-1) to let the algo remove outgoing characters

    vector<int> ans;

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
            bool match = true;
            
            for (int j = 0; j < m; j ++){ // We must manually check in case of a hash collision
                if (text[i + j] != pattern[j]){
                    match = false;
                    break;
                }
            }

            if (match)
                ans.push_back(i);
        }

        if (i < n - m){ // Calculate the hash of the next windows without hashing all characters again
            t = (d * (t - text[i] * h) + text[i + m]) % q;

            if (t < 0) // Basically converts a negative hash to the positive equivalent
                t += q;
        }
    }

    return ans;
}

// Functions to generate and select a random prime number within a range 
// to use as the modulus for hashing

vector<int> generate_primes(int limit){
    vector<bool> is_prime(limit + 1, true);

    is_prime[0] = is_prime[1] = false;

    for (int i = 2; i * i <= limit; i++){
        if (is_prime[i]){
            for (int j = i * i; j <= limit; j += i){
                is_prime[j] = false;
            }
        }
    }

    vector<int> primes;

    for (int i = 2; i <= limit; i++){
        if (is_prime[i]){
            primes.push_back(i);
        }
    }

    return primes;
}

const vector<int>& get_primes() {
    static const vector<int> primes = generate_primes(100000); // Only get primes in range once
    return primes;
}

int random_prime(int min_prime, int max_prime){
    static mt19937 rng(random_device{}());

    const vector<int>& primes = get_primes();

    auto first = lower_bound(primes.begin(), primes.end(), min_prime); // find the first prime >= to min_prime
    auto last = upper_bound(primes.begin(), primes.end(), max_prime); // find the first prime strictly > than max+prime

    if (first == last){
        throw invalid_argument("No primes in specified range.");
    }

    uniform_int_distribution<int> dist(0, static_cast<int>(last - first) - 1);

    return *(first + dist(rng));
}

// using g++ version 15.20
// build with g++ -std=c++20 src/rabin_karp.cpp -o main.exe
// prints the elemnts of a vector of indices where matches occur
int main(){ // TEMPORARY: Will be removed later when benchmarking happens and both algorithms are used together
    string txt = "bananabanana";
    string pat = "banana";
    int q = random_prime(10000, 100000);  // Prime no. for modulo operations -> This is where the randomised component comes from
    vector<int> res = rabin_karp(pat, txt, q); 
    for (auto it : res){
        cout << it << " ";
    }
    cout << '\n';
    
    cin.get();
    return 0;
}