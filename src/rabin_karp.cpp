/*
Rabin-Karp Algorithm
Hannah Effat Manesh Nik
25809221
*/

#include <vector>
#include <string>
#include <iostream>
using namespace std;

vector<int> rabin_karp(const string &pattern, const string &text){

    
    if (pattern.empty() || pattern.length() > text.length())
        return {-1};

    int d = 256; // No. of chars in input alphabet - assumed ASCII
    int q = 101; // Prime no. for modulo operations

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

            if (t < 0)
                t += q;
        }
    }

    return ans;
}

int main(){ // Will probably be removed later when benchmarking happens and both algorithms are used together
    string txt = "bananabanana";
    string pat = "banana";
    vector<int> res = rabin_karp(pat, txt);
    for (auto it : res){
        cout << it << " ";
    }
    cout << '\n';
    
    cin.get();
    return 0;
}