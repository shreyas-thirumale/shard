#include "Parser.h"

#include "../ast/CommandNodes.h"

#include <stdexcept>

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_pos(0) {
}

ASTNodePtr Parser::parse() {
    if (atEnd()) return nullptr;
    ASTNodePtr node = parseCommand();
    return node;
}

// ── Token navigation ──────────────────────────────────────────────────────────

const Token& Parser::current() const {
    return m_tokens[m_pos];
}

const Token& Parser::peek(std::size_t offset) const {
    std::size_t idx = m_pos + offset;
    if (idx >= m_tokens.size()) return m_tokens.back(); // END_OF_INPUT
    return m_tokens[idx];
}

const Token& Parser::consume() {
    const Token& t = m_tokens[m_pos];
    if (!atEnd()) ++m_pos;
    return t;
}

bool Parser::atEnd() const {
    return m_tokens[m_pos].type == TokenType::END_OF_INPUT;
}

bool Parser::isKeyword(const std::string& value) const {
    return current().type == TokenType::KEYWORD && current().value == value;
}

std::string Parser::expectIdentifier() {
    if (current().type == TokenType::IDENTIFIER || current().type == TokenType::STRING) {
        return consume().value;
    }
    // Also accept keywords used as filenames (e.g. "move file called 'list'")
    if (current().type == TokenType::KEYWORD) {
        return consume().value;
    }
    throw ParseError("Expected a file or directory name", current().position, true);
}

// ── Top-level dispatch ────────────────────────────────────────────────────────

ASTNodePtr Parser::parseCommand() {
    if (current().type != TokenType::KEYWORD) {
        throw ParseError(
            "Unknown command: '" + current().value + "'. Type 'help' to see available commands.",
            current().position,
            false
        );
    }

    const std::string& kw = current().value;

    // Navigation
    if (kw == "change" || kw == "go" || kw == "navigate" || kw == "enter") {
        return parseChangeDirectory();
    }

    // "show current directory" or "list files"
    if (kw == "show") {
        if (peek().type == TokenType::KEYWORD && peek().value == "current") {
            return parseCurrentDirectory();
        }
        return parseListDirectory();
    }

    if (kw == "list") {
        return parseListDirectory();
    }

    // File ops
    if (kw == "copy") return parseCopyFile();
    if (kw == "move") return parseMoveFile();
    if (kw == "delete" || kw == "remove") {
        // Look ahead to distinguish file vs directory
        // "delete folder X" / "remove directory X" → DeleteDirectory
        // "delete file X" / "delete X" → DeleteFile
        if (peek().type == TokenType::KEYWORD &&
            (peek().value == "folder" || peek().value == "directory")) {
            return parseDeleteDirectory();
        }
        return parseDeleteFile();
    }
    if (kw == "rename") return parseRenameFile();

    // "create" and "make" dispatch on next token
    if (kw == "create" || kw == "make") {
        if (peek().type == TokenType::KEYWORD &&
            (peek().value == "folder" || peek().value == "directory")) {
            return parseCreateDirectory();
        }
        return parseCreateFile();
    }

    // Meta
    if (kw == "help") return parseHelp();
    if (kw == "exit" || kw == "quit") return parseExit();

    throw ParseError(
            "Unknown command: '" + kw + "'. Type 'help' to see available commands.",
            current().position,
            false
        );
}

// ── Navigation ────────────────────────────────────────────────────────────────

ASTNodePtr Parser::parseChangeDirectory() {
    // Accepted patterns:
    //   change directory <path>
    //   change directory to <path>
    //   go to <path>
    //   go back
    //   navigate into <path>
    //   enter <path>

    consume(); // consume the leading verb (change / go / navigate / enter)

    // Optional "directory" / "folder" after "change"
    if (current().type == TokenType::KEYWORD &&
        (current().value == "directory" || current().value == "folder")) {
        consume();
    }

    // Optional preposition: to / into / back
    if (current().type == TokenType::KEYWORD) {
        if (current().value == "back") {
            consume();
            return std::make_unique<ChangeDirectoryNode>("..");
        }
        if (current().value == "to" || current().value == "into") {
            consume();
        }
    }

    // Handle ".." shorthand
    if (current().type == TokenType::DOTDOT) {
        consume();
        return std::make_unique<ChangeDirectoryNode>("..");
    }

    std::string dest = expectIdentifier();
    return std::make_unique<ChangeDirectoryNode>(dest);
}

ASTNodePtr Parser::parseCurrentDirectory() {
    // show current directory
    consume(); // show
    if (isKeyword("current")) consume();
    if (isKeyword("directory") || isKeyword("folder")) consume();
    return std::make_unique<CurrentDirectoryNode>();
}

ASTNodePtr Parser::parseListDirectory() {
    // list files
    // list directory
    // show files
    consume(); // list / show

    // Optional: "files" / "directory" / "folder"
    if (current().type == TokenType::KEYWORD &&
        (current().value == "files" || current().value == "file" ||
         current().value == "directory" || current().value == "folder")) {
        consume();
    }

    // Optional path argument
    std::string path;
    if (!atEnd() && (current().type == TokenType::IDENTIFIER ||
                     current().type == TokenType::STRING)) {
        path = consume().value;
    }

    return std::make_unique<ListDirectoryNode>(path);
}

// ── File Operations ───────────────────────────────────────────────────────────

ASTNodePtr Parser::parseCopyFile() {
    // copy <src> to <dest>
    // copy file <src> to <dest>
    // copy <src> into <dest>
    consume(); // copy

    if (isKeyword("file")) consume(); // optional "file"

    std::string src = expectIdentifier();

    // Expect "to" or "into"
    if (current().type == TokenType::KEYWORD &&
        (current().value == "to" || current().value == "into")) {
        consume();
    } else {
        throw ParseError(
            "Expected 'to' or 'into' after source file — usage: copy <file> to <destination>",
            current().position,
            true
        );
    }

    std::string dest = expectIdentifier();
    return std::make_unique<CopyFileNode>(src, dest);
}

ASTNodePtr Parser::parseMoveFile() {
    // move <src> to <dest>
    // move <src> into <dest>
    // move file <src> to <dest>
    consume(); // move

    if (isKeyword("file")) consume();

    std::string src = expectIdentifier();

    if (current().type == TokenType::KEYWORD &&
        (current().value == "to" || current().value == "into")) {
        consume();
    } else {
        throw ParseError(
            "Expected 'to' or 'into' after source file — usage: move <file> to <destination>",
            current().position,
            true
        );
    }

    std::string dest = expectIdentifier();
    return std::make_unique<MoveFileNode>(src, dest);
}

ASTNodePtr Parser::parseDeleteFile() {
    // delete <file>
    // remove <file>
    // delete file <file>
    // remove file <file>
    consume(); // delete / remove

    if (isKeyword("file")) consume();

    std::string target = expectIdentifier();
    return std::make_unique<DeleteFileNode>(target);
}

ASTNodePtr Parser::parseRenameFile() {
    // rename <src> to <newname>
    // rename <src> as <newname>
    // rename file <src> to <newname>
    consume(); // rename

    if (isKeyword("file")) consume();

    std::string src = expectIdentifier();

    if (current().type == TokenType::KEYWORD &&
        (current().value == "to" || current().value == "as")) {
        consume();
    } else {
        throw ParseError(
            "Expected 'to' or 'as' after source name — usage: rename <file> to <newname>",
            current().position,
            true
        );
    }

    std::string name = expectIdentifier();
    return std::make_unique<RenameFileNode>(src, name);
}

ASTNodePtr Parser::parseCreateFile() {
    // create file <name>
    // make file <name>
    consume(); // create / make

    if (isKeyword("file")) consume();

    std::string name = expectIdentifier();
    return std::make_unique<CreateFileNode>(name);
}

// ── Directory Operations ──────────────────────────────────────────────────────

ASTNodePtr Parser::parseCreateDirectory() {
    // create folder <name>
    // create directory <name>
    // make folder <name>
    // make directory <name>
    consume(); // create / make
    consume(); // folder / directory

    std::string name = expectIdentifier();
    return std::make_unique<CreateDirectoryNode>(name);
}

ASTNodePtr Parser::parseDeleteDirectory() {
    // delete folder <name>
    // remove directory <name>
    consume(); // delete / remove
    consume(); // folder / directory

    std::string target = expectIdentifier();
    return std::make_unique<DeleteDirectoryNode>(target);
}

ASTNodePtr Parser::parseRenameDirectory() {
    // rename folder <src> to <name>
    // rename directory <src> to <name>
    consume(); // rename
    consume(); // folder / directory

    std::string src = expectIdentifier();

    if (current().type == TokenType::KEYWORD &&
        (current().value == "to" || current().value == "as")) {
        consume();
    } else {
        throw ParseError(
            "Expected 'to' or 'as' after directory name — usage: rename folder <name> to <newname>",
            current().position,
            true
        );
    }

    std::string name = expectIdentifier();
    return std::make_unique<RenameDirectoryNode>(src, name);
}

// ── Meta ──────────────────────────────────────────────────────────────────────

ASTNodePtr Parser::parseHelp() {
    consume(); // help

    std::string topic;
    if (!atEnd() && (current().type == TokenType::KEYWORD ||
                     current().type == TokenType::IDENTIFIER)) {
        topic = consume().value;
    }

    return std::make_unique<HelpNode>(topic);
}

ASTNodePtr Parser::parseExit() {
    consume(); // exit / quit
    return std::make_unique<ExitNode>();
}
