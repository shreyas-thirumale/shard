#pragma once

#include <string>
#include <vector>

namespace StringUtils {

// Trims leading and trailing whitespace.
std::string trim(const std::string& s);

// Converts a string to lowercase.
std::string toLower(const std::string& s);

// Splits a string by a delimiter.
std::vector<std::string> split(const std::string& s, char delimiter);

// Returns the Levenshtein edit distance between two strings.
// Used by fuzzy matching in the parser error recovery path.
int editDistance(const std::string& a, const std::string& b);

} // namespace StringUtils
