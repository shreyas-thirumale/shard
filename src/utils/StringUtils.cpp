#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace StringUtils {

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string toLower(const std::string& s) {
    std::string result = s;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> parts;
    std::istringstream stream(s);
    std::string part;
    while (std::getline(stream, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

int editDistance(const std::string& a, const std::string& b) {
    const std::size_t m = a.size();
    const std::size_t n = b.size();

    // dp[i][j] = edit distance between a[0..i-1] and b[0..j-1]
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (std::size_t i = 0; i <= m; ++i) dp[i][0] = static_cast<int>(i);
    for (std::size_t j = 0; j <= n; ++j) dp[0][j] = static_cast<int>(j);

    for (std::size_t i = 1; i <= m; ++i) {
        for (std::size_t j = 1; j <= n; ++j) {
            if (std::tolower(static_cast<unsigned char>(a[i - 1])) ==
                std::tolower(static_cast<unsigned char>(b[j - 1]))) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }

    return dp[m][n];
}

} // namespace StringUtils
