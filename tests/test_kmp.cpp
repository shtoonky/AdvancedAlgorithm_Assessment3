
#include "kmp.hpp"

#include <cassert>
#include <iostream>
#include <vector>

int main()
{
    assert(build_lps("ababaca") ==
           std::vector<int>({0, 0, 1, 2, 3, 0, 1}));

    assert(build_lps("aaaa") ==
           std::vector<int>({0, 1, 2, 3}));

    assert(build_lps("abcde") ==
           std::vector<int>({0, 0, 0, 0, 0}));

    assert(build_lps("").empty());

    assert(kmp_search("abra", "abracadabra") ==
           std::vector<int>({0, 7}));

    assert(kmp_search("aaa", "aaaaa") ==
           std::vector<int>({0, 1, 2}));

    assert(kmp_search("aba", "ababa") ==
           std::vector<int>({0, 2}));

    assert(kmp_search("xyz", "abcde").empty());

    assert(kmp_search("abc", "abc") ==
           std::vector<int>({0}));

    assert(kmp_search("abcd", "abc").empty());

    assert(kmp_search("", "abc").empty());
    assert(kmp_search("a", "").empty());

    std::cout << "PASS: KMP preprocessing and basic search tests\n";

    return 0;
}
