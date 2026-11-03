
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

std::vector<int> kmp_search(const std::string &pattern,
                            const std::string &text)
{
    std::vector<int> matches;
    const int m = static_cast<int>(pattern.size());
    const int n = static_cast<int>(text.size());

    if (m == 0 || m > n)
    {
        return matches;
    }

    const std::vector<int> lps = build_lps(pattern);
    int i = 0; // current text position
    int j = 0; // no. of pattern characters matched

    while (i < n)
    {
        if (text[i] == pattern[j])
        {
            ++i;
            ++j;

            if (j == m)
            {
                matches.push_back(i - m);

                j = lps[j - 1];
            }
        }
        else if (j > 0)
        {
            j = lps[j - 1];
        }
        else
        {
            ++i;
        }
    }

    return matches;
}
