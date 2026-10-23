#ifndef RABIN_KARP_HPP
#define RABIN_KARP_HPP

#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

vector<int> rabin_karp(const string& pattern, const string& text, int q);

int random_prime(int min_prime, int max_prime);

#endif