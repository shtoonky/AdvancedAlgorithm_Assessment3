#include <vector>
#include <string>
#include <iostream>
using namespace std;

vector<int> rabin_karp(const string &pattern, const string &text){

    
    int d = 256; // No. of chars in input alphabet - assumed ASCII
    int q = 101; // Prime no. for modulo operations

    int m = pattern.length();
    int n = text.length();

    int p = 0; // Hash value for pattern
    int t = 0; // Hash value for current window of text

    int h = 1; // High order digit multiplier

    vector<int> ans;

    // Precompute h 
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    // Compute initial hash values for pattern and first window of text
    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    // Slide pattern over text by one
    for (int i = 0; i <= n - m; i++){
        
        // If hash values match, check characters
        if (p == t){
            bool match = true;
            
            for (int j = 0; j < m; j ++){

                if (text[i + j] != pattern[j]){
                    match = false;
                    break;
                }
            }
            if (match)
            ans.push_back(i);
        }

        if (i < n - m){
            t = (d * (t - text[i] * h) + text[i + m]) % q;

            if (t < 0)
                t += q;
        }
    }

    return ans;
}

int main(){
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