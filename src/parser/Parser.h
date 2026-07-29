#pragma once

#include "../ast/ASTNode.h"
#include "../lexer/Token.h"

#include <string>
#include <vector>

// ParseError is thrown when the parser cannot match any known command pattern.
struct ParseError {
    std::string message;
    std::size_t position;

    ParseError(std::string msg, std::size_t pos)
        : message(std::move(msg)), position(pos) {}
};

// Parser consumes a token list and produces an AST node.
// It is deterministic — no AI, no guessing.
// If input cannot be matched, it throws ParseError.
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // Entry point. Returns the root AST node for the input.
    ASTNodePtr parse();

private:
    std::vector<Token> m_tokens;
    std::size_t m_pos;

    // Token navigation
    const Token& current() const;
    const Token& peek(std::size_t offset = 1) const;
    const Token& consume();
    bool atEnd() const;

    // Checks if current token matches a keyword value
    bool isKeyword(const std::string& value) const;

    // Consumes a token and returns its value, or throws if not IDENTIFIER/STRING
    std::string expectIdentifier();

    // Top-level dispatch — looks at the first keyword and routes accordingly
    ASTNodePtr parseCommand();

    // ── Navigation ────────────────────────────────────────────────────────────
    ASTNodePtr parseChangeDirectory();  // change / go / navigate / enter
    ASTNodePtr parseCurrentDirectory(); // show current directory / where am i
    ASTNodePtr parseListDirectory();    // list files / show files

    // ── File Operations ───────────────────────────────────────────────────────
    ASTNodePtr parseCopyFile();         // copy <src> to <dest>
    ASTNodePtr parseMoveFile();         // move <src> to <dest>
    ASTNodePtr parseDeleteFile();       // delete / remove <target>
    ASTNodePtr parseRenameFile();       // rename <src> to/as <name>
    ASTNodePtr parseCreateFile();       // create file / make file <name>

    // ── Directory Operations ──────────────────────────────────────────────────
    ASTNodePtr parseCreateDirectory();  // create folder / make directory <name>
    ASTNodePtr parseDeleteDirectory();  // remove directory / delete folder <name>
    ASTNodePtr parseRenameDirectory();  // rename folder <src> to <name>

    // ── Meta ──────────────────────────────────────────────────────────────────
    ASTNodePtr parseHelp();
    ASTNodePtr parseExit();
};
