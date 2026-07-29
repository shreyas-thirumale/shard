#pragma once

#include "ASTNode.h"
#include <string>

// ── Navigation ────────────────────────────────────────────────────────────────

// change directory Documents
// go to Documents
// navigate into Documents
// enter Documents
struct ChangeDirectoryNode : public ASTNode {
    std::string destination; // Target path or ".." for back

    explicit ChangeDirectoryNode(std::string dest)
        : destination(std::move(dest)) {}

    std::string toString() const override {
        return "ChangeDirectory { destination: " + destination + " }";
    }
};

// show current directory
// where am i
struct CurrentDirectoryNode : public ASTNode {
    std::string toString() const override {
        return "CurrentDirectory {}";
    }
};

// list files
// show files
// list directory
struct ListDirectoryNode : public ASTNode {
    std::string path; // Empty means current directory

    explicit ListDirectoryNode(std::string path = "")
        : path(std::move(path)) {}

    std::string toString() const override {
        return "ListDirectory { path: " + (path.empty() ? "." : path) + " }";
    }
};

// ── File Operations ───────────────────────────────────────────────────────────

// copy report.pdf to Backup
// copy report.pdf into Backup
struct CopyFileNode : public ASTNode {
    std::string source;
    std::string destination;

    CopyFileNode(std::string src, std::string dest)
        : source(std::move(src)), destination(std::move(dest)) {}

    std::string toString() const override {
        return "CopyFile { source: " + source + ", destination: " + destination + " }";
    }
};

// move image.png into Pictures
// move image.png to Pictures
struct MoveFileNode : public ASTNode {
    std::string source;
    std::string destination;

    MoveFileNode(std::string src, std::string dest)
        : source(std::move(src)), destination(std::move(dest)) {}

    std::string toString() const override {
        return "MoveFile { source: " + source + ", destination: " + destination + " }";
    }
};

// delete old.txt
// remove old.txt
struct DeleteFileNode : public ASTNode {
    std::string target;

    explicit DeleteFileNode(std::string target)
        : target(std::move(target)) {}

    std::string toString() const override {
        return "DeleteFile { target: " + target + " }";
    }
};

// rename old.txt as new.txt
// rename old.txt to new.txt
struct RenameFileNode : public ASTNode {
    std::string source;
    std::string newName;

    RenameFileNode(std::string src, std::string name)
        : source(std::move(src)), newName(std::move(name)) {}

    std::string toString() const override {
        return "RenameFile { source: " + source + ", newName: " + newName + " }";
    }
};

// create file notes.txt
// make file notes.txt
struct CreateFileNode : public ASTNode {
    std::string name;

    explicit CreateFileNode(std::string name)
        : name(std::move(name)) {}

    std::string toString() const override {
        return "CreateFile { name: " + name + " }";
    }
};

// ── Directory Operations ──────────────────────────────────────────────────────

// create folder Projects
// make folder Projects
// make directory Projects
struct CreateDirectoryNode : public ASTNode {
    std::string name;

    explicit CreateDirectoryNode(std::string name)
        : name(std::move(name)) {}

    std::string toString() const override {
        return "CreateDirectory { name: " + name + " }";
    }
};

// remove directory Temp
// delete folder Temp
struct DeleteDirectoryNode : public ASTNode {
    std::string target;

    explicit DeleteDirectoryNode(std::string target)
        : target(std::move(target)) {}

    std::string toString() const override {
        return "DeleteDirectory { target: " + target + " }";
    }
};

// rename folder OldName to NewName
struct RenameDirectoryNode : public ASTNode {
    std::string source;
    std::string newName;

    RenameDirectoryNode(std::string src, std::string name)
        : source(std::move(src)), newName(std::move(name)) {}

    std::string toString() const override {
        return "RenameDirectory { source: " + source + ", newName: " + newName + " }";
    }
};

// ── Meta ──────────────────────────────────────────────────────────────────────

// help
// help copy
struct HelpNode : public ASTNode {
    std::string topic; // Empty means top-level help

    explicit HelpNode(std::string topic = "")
        : topic(std::move(topic)) {}

    std::string toString() const override {
        return "Help { topic: " + (topic.empty() ? "(general)" : topic) + " }";
    }
};

// exit / quit
struct ExitNode : public ASTNode {
    std::string toString() const override {
        return "Exit {}";
    }
};
