
#ifndef KMP_HPP
#define KMP_HPP

#include <string>
#include <vector>

std::vector<int> build_lps(const std::string &pattern);

std::vector<int> kmp_search(const std::string &pattern,
                            const std::string &text);

#endif
