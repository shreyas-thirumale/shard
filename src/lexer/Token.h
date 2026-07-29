#pragma once

#include <string>

// Every piece of user input gets broken into tokens.
// TokenType defines what kind of thing a token represents.
enum class TokenType {
    // Action keywords
    KEYWORD,        // change, go, copy, move, delete, create, list, show, enter, navigate

    // Generic identifiers (filenames, directory names, etc.)
    IDENTIFIER,

    // Prepositions and connectors used in command patterns
    PREPOSITION,    // to, into, from, as

    // Punctuation / structure
    DOT,            // .
    DOTDOT,         // ..
    SLASH,          // /
    WILDCARD,       // *

    // Quoted string literals
    STRING,         // "hello world"

    // Flags / options (future use)
    FLAG,           // --recursive, -r

    // End of input
    END_OF_INPUT,

    // Unrecognized token — triggers error handling
    UNKNOWN
};

struct Token {
    TokenType   type;
    std::string value;      // The raw text of the token
    std::size_t position;   // Column offset in the original input (for error messages)

    Token(TokenType type, std::string value, std::size_t position)
        : type(type), value(std::move(value)), position(position) {}
};
