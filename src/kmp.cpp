
#include "kmp.hpp"

std::vector<int> build_lps(const std::string &pattern)
{
    const int m = static_cast<int>(pattern.size());
    std::vector<int> lps(m, 0);

    int length = 0;
    int i = 1;

    while (i < m)
    {
        if (pattern[i] == pattern[length])
        {
            ++length;
            lps[i] = length;
            ++i;
        }
        else if (length > 0)
        {
            // try shorter matching prefix
            length = lps[length - 1];
        }
        else
        {
            lps[i] = 0;
            ++i;
        }
    }

    return lps;
}
