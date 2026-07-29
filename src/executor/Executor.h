#pragma once

#include "../ast/ASTNode.h"
#include "../commands/ICommand.h"

// Executor walks the AST and runs the appropriate command.
// It holds the current execution context (cwd, env, etc.)
// and updates it as commands run.
class Executor {
public:
    Executor();

    // Executes the given AST node and returns the result.
    CommandResult execute(const ASTNode* node);

    const ExecutionContext& context() const { return m_context; }

private:
    ExecutionContext m_context;
};
