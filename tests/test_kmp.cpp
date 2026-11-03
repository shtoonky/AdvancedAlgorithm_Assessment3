
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

    std::cout << "PASS: KMP LPS preprocessing tests\n";

    return 0;
}
