#pragma once

#include "Token.h"

#include <string>
#include <vector>

// Lexer takes raw input and produces a flat list of tokens.
// It does not assign meaning — that is the parser's job.
class Lexer {
public:
    explicit Lexer(std::string input);

    // Tokenizes the full input and returns the token list.
    std::vector<Token> tokenize();

private:
    std::string m_input;
    std::size_t m_pos;

    char current() const;
    char peek() const;
    void advance();
    bool atEnd() const;

    void skipWhitespace();

    Token readKeywordOrIdentifier();
    Token readString();       // Handles quoted strings
    Token readFlag();         // Handles --flag or -f style args
};
