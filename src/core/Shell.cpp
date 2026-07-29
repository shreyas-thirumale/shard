#include "Shell.h"

#include "../lexer/Lexer.h"
#include "../parser/Parser.h"
#include "../executor/Executor.h"

#include <iostream>
#include <string>

Shell::Shell()
    : m_running(false)
    , m_executor(std::make_unique<Executor>()) {
}

void Shell::run() {
    m_running = true;

    std::cout << "Shard v0.1.0\n";
    std::cout << "Type 'help' for available commands, 'exit' to quit.\n\n";

    while (m_running) {
        printPrompt();

        std::string input = readInput();

        if (input.empty()) {
            continue;
        }

        processInput(input);
    }
}

std::string Shell::readInput() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        // EOF (Ctrl+D)
        m_running = false;
        std::cout << "\n";
        return "";
    }
    return line;
}

void Shell::processInput(const std::string& input) {
    // ── Lex ───────────────────────────────────────────────────────────────────
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    // ── Parse ─────────────────────────────────────────────────────────────────
    ASTNodePtr ast;
    try {
        Parser parser(std::move(tokens));
        ast = parser.parse();
    } catch (const ParseError& e) {
        std::cout << "  Unknown command: '" << input << "'\n";
        std::cout << "  " << e.message << "\n\n";
        return;
    }

    if (!ast) return;

    // ── Execute ───────────────────────────────────────────────────────────────
    CommandResult result = m_executor->execute(ast.get());

    if (result.success) {
        // Special signal from ExitNode
        if (result.output == "__exit__") {
            m_running = false;
            std::cout << "Goodbye.\n";
            return;
        }
        if (!result.output.empty()) {
            std::cout << result.output;
            // Ensure trailing newline
            if (result.output.back() != '\n') std::cout << "\n";
        }
    } else {
        std::cout << "  Error: " << result.error << "\n";
    }

    std::cout << "\n";
}

void Shell::printPrompt() const {
    // Show current directory basename in the prompt
    const std::string& cwd = m_executor->context().currentDirectory;
    std::string dirname = cwd;

    auto slash = cwd.rfind('/');
    if (slash != std::string::npos && slash + 1 < cwd.size()) {
        dirname = cwd.substr(slash + 1);
    }

    std::cout << dirname << " » " << std::flush;
}
