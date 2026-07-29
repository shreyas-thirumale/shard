#include "Lexer.h"

#include <cctype>
#include <stdexcept>
#include <unordered_set>

// Keywords recognized by the lexer.
// These are the action words and connectors that Shard understands.
static const std::unordered_set<std::string> KEYWORDS = {
    // Navigation
    "change", "go", "navigate", "enter",
    // File ops
    "copy", "move", "delete", "remove", "rename", "create",
    // Directory ops
    "make", "list", "show",
    // Connectors
    "directory", "folder", "file", "files",
    // Prepositions
    "to", "into", "from", "as", "back",
    // Meta
    "help", "exit", "quit", "current"
};

Lexer::Lexer(std::string input)
    : m_input(std::move(input)), m_pos(0) {
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!atEnd()) {
        skipWhitespace();

        if (atEnd()) break;

        char c = current();

        if (c == '"' || c == '\'') {
            tokens.push_back(readString());
        } else if (c == '-') {
            tokens.push_back(readFlag());
        } else if (c == '.') {
            if (peek() == '.') {
                tokens.emplace_back(TokenType::DOTDOT, "..", m_pos);
                advance();
                advance();
            } else {
                tokens.emplace_back(TokenType::DOT, ".", m_pos);
                advance();
            }
        } else if (c == '/') {
            tokens.emplace_back(TokenType::SLASH, "/", m_pos);
            advance();
        } else if (c == '*') {
            tokens.emplace_back(TokenType::WILDCARD, "*", m_pos);
            advance();
        } else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(readKeywordOrIdentifier());
        } else {
            // Unknown character — record position for error reporting
            tokens.emplace_back(TokenType::UNKNOWN, std::string(1, c), m_pos);
            advance();
        }
    }

    tokens.emplace_back(TokenType::END_OF_INPUT, "", m_pos);
    return tokens;
}

char Lexer::current() const {
    return m_input[m_pos];
}

char Lexer::peek() const {
    if (m_pos + 1 < m_input.size()) return m_input[m_pos + 1];
    return '\0';
}

void Lexer::advance() {
    ++m_pos;
}

bool Lexer::atEnd() const {
    return m_pos >= m_input.size();
}

void Lexer::skipWhitespace() {
    while (!atEnd() && std::isspace(static_cast<unsigned char>(current()))) {
        advance();
    }
}

Token Lexer::readKeywordOrIdentifier() {
    std::size_t start = m_pos;
    std::string word;

    // Read alphanumeric characters, underscores, dots (for filenames), hyphens
    while (!atEnd()) {
        char c = current();
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.' || c == '-') {
            word += c;
            advance();
        } else {
            break;
        }
    }

    // Lowercase for keyword matching
    std::string lower = word;
    for (char& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

    if (KEYWORDS.count(lower)) {
        return Token(TokenType::KEYWORD, lower, start);
    }

    return Token(TokenType::IDENTIFIER, word, start);
}

Token Lexer::readString() {
    char quote = current();
    advance(); // skip opening quote

    std::size_t start = m_pos;
    std::string value;

    while (!atEnd() && current() != quote) {
        if (current() == '\\') {
            advance(); // skip backslash
            if (!atEnd()) {
                value += current();
                advance();
            }
        } else {
            value += current();
            advance();
        }
    }

    if (!atEnd()) advance(); // skip closing quote

    return Token(TokenType::STRING, value, start);
}

Token Lexer::readFlag() {
    std::size_t start = m_pos;
    std::string value;

    value += current(); // leading '-'
    advance();

    if (!atEnd() && current() == '-') {
        value += current(); // second '-' for --flags
        advance();
    }

    while (!atEnd() && (std::isalnum(static_cast<unsigned char>(current())) || current() == '-')) {
        value += current();
        advance();
    }

    return Token(TokenType::FLAG, value, start);
}
