#pragma once

#include <memory>
#include <string>
#include <vector>

// Base class for all AST nodes.
// Every parsed command becomes a subclass of ASTNode.
class ASTNode {
public:
    virtual ~ASTNode() = default;

    // Returns a human-readable description of the node (used for debugging).
    virtual std::string toString() const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;
