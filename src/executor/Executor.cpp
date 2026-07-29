#include "Executor.h"

#include "../ast/CommandNodes.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

Executor::Executor() {
    m_context.currentDirectory = fs::current_path().string();
}

CommandResult Executor::execute(const ASTNode* node) {
    if (!node) {
        return CommandResult::fail("Nothing to execute.");
    }

    // ── Navigation ────────────────────────────────────────────────────────────

    if (auto* n = dynamic_cast<const ChangeDirectoryNode*>(node)) {
        fs::path target;

        if (n->destination == "..") {
            target = fs::path(m_context.currentDirectory).parent_path();
        } else if (fs::path(n->destination).is_absolute()) {
            target = n->destination;
        } else {
            target = fs::path(m_context.currentDirectory) / n->destination;
        }

        std::error_code ec;
        target = fs::canonical(target, ec);

        if (ec || !fs::is_directory(target, ec)) {
            return CommandResult::fail(
                "Directory not found: '" + n->destination + "'"
            );
        }

        m_context.currentDirectory = target.string();
        return CommandResult::ok();
    }

    if (dynamic_cast<const CurrentDirectoryNode*>(node)) {
        return CommandResult::ok(m_context.currentDirectory);
    }

    if (auto* n = dynamic_cast<const ListDirectoryNode*>(node)) {
        fs::path target = n->path.empty()
            ? fs::path(m_context.currentDirectory)
            : fs::path(m_context.currentDirectory) / n->path;

        std::error_code ec;
        if (!fs::is_directory(target, ec)) {
            return CommandResult::fail("Not a directory: '" + target.string() + "'");
        }

        std::string output;
        for (const auto& entry : fs::directory_iterator(target, ec)) {
            if (ec) break;
            const auto& p = entry.path();
            std::string name = p.filename().string();
            if (fs::is_directory(p)) name += "/";
            output += name + "\n";
        }

        return CommandResult::ok(output);
    }

    // ── File Operations ───────────────────────────────────────────────────────

    if (auto* n = dynamic_cast<const CopyFileNode*>(node)) {
        fs::path src  = fs::path(m_context.currentDirectory) / n->source;
        fs::path dest = fs::path(m_context.currentDirectory) / n->destination;

        std::error_code ec;
        fs::copy(src, dest, fs::copy_options::overwrite_existing, ec);

        if (ec) {
            return CommandResult::fail("Could not copy: " + ec.message());
        }
        return CommandResult::ok("Copied '" + n->source + "' to '" + n->destination + "'");
    }

    if (auto* n = dynamic_cast<const MoveFileNode*>(node)) {
        fs::path src  = fs::path(m_context.currentDirectory) / n->source;
        fs::path dest = fs::path(m_context.currentDirectory) / n->destination;

        std::error_code ec;
        fs::rename(src, dest, ec);

        if (ec) {
            return CommandResult::fail("Could not move: " + ec.message());
        }
        return CommandResult::ok("Moved '" + n->source + "' to '" + n->destination + "'");
    }

    if (auto* n = dynamic_cast<const DeleteFileNode*>(node)) {
        fs::path target = fs::path(m_context.currentDirectory) / n->target;

        std::error_code ec;
        fs::remove(target, ec);

        if (ec) {
            return CommandResult::fail("Could not delete: " + ec.message());
        }
        return CommandResult::ok("Deleted '" + n->target + "'");
    }

    if (auto* n = dynamic_cast<const RenameFileNode*>(node)) {
        fs::path src  = fs::path(m_context.currentDirectory) / n->source;
        fs::path dest = fs::path(m_context.currentDirectory) / n->newName;

        std::error_code ec;
        fs::rename(src, dest, ec);

        if (ec) {
            return CommandResult::fail("Could not rename: " + ec.message());
        }
        return CommandResult::ok("Renamed '" + n->source + "' to '" + n->newName + "'");
    }

    if (auto* n = dynamic_cast<const CreateFileNode*>(node)) {
        fs::path target = fs::path(m_context.currentDirectory) / n->name;

        if (fs::exists(target)) {
            return CommandResult::fail("File already exists: '" + n->name + "'");
        }

        // Create empty file
        std::ofstream f(target);
        if (!f) {
            return CommandResult::fail("Could not create file: '" + n->name + "'");
        }
        return CommandResult::ok("Created '" + n->name + "'");
    }

    // ── Directory Operations ──────────────────────────────────────────────────

    if (auto* n = dynamic_cast<const CreateDirectoryNode*>(node)) {
        fs::path target = fs::path(m_context.currentDirectory) / n->name;

        std::error_code ec;
        fs::create_directory(target, ec);

        if (ec) {
            return CommandResult::fail("Could not create directory: " + ec.message());
        }
        return CommandResult::ok("Created folder '" + n->name + "'");
    }

    if (auto* n = dynamic_cast<const DeleteDirectoryNode*>(node)) {
        fs::path target = fs::path(m_context.currentDirectory) / n->target;

        std::error_code ec;
        fs::remove_all(target, ec);

        if (ec) {
            return CommandResult::fail("Could not delete directory: " + ec.message());
        }
        return CommandResult::ok("Deleted folder '" + n->target + "'");
    }

    if (auto* n = dynamic_cast<const RenameDirectoryNode*>(node)) {
        fs::path src  = fs::path(m_context.currentDirectory) / n->source;
        fs::path dest = fs::path(m_context.currentDirectory) / n->newName;

        std::error_code ec;
        fs::rename(src, dest, ec);

        if (ec) {
            return CommandResult::fail("Could not rename directory: " + ec.message());
        }
        return CommandResult::ok("Renamed folder '" + n->source + "' to '" + n->newName + "'");
    }

    // ── Meta ──────────────────────────────────────────────────────────────────

    if (dynamic_cast<const HelpNode*>(node)) {
        // Stub — help system will be implemented in Milestone 4
        return CommandResult::ok(
            "Shard Help (coming soon)\n"
            "  Navigation:  go to <dir>  |  list files  |  show current directory\n"
            "  Files:       copy <src> to <dest>  |  move  |  delete  |  rename  |  create file\n"
            "  Directories: create folder <name>  |  delete folder  |  rename folder\n"
            "  Meta:        help  |  exit\n"
        );
    }

    if (dynamic_cast<const ExitNode*>(node)) {
        // Signal handled in Shell
        return CommandResult::ok("__exit__");
    }

    return CommandResult::fail("Unhandled command node: " + node->toString());
}
