#pragma once

#include <string>
#include <vector>

// ExecutionContext carries runtime state into a command's execute() call.
// Commands should not reach outside this struct for environment information.
struct ExecutionContext {
    std::string currentDirectory;
    // Future: environment variables, shell flags, etc.
};

// CommandResult communicates the outcome of a command back to the shell.
struct CommandResult {
    bool        success;
    std::string output;     // Text to display to the user on success
    std::string error;      // Error message to display on failure

    static CommandResult ok(std::string output = "") {
        return {true, std::move(output), ""};
    }

    static CommandResult fail(std::string error) {
        return {false, "", std::move(error)};
    }
};

// ICommand is the interface every Shard command implements.
// The command registry holds pointers to ICommand instances.
class ICommand {
public:
    virtual ~ICommand() = default;

    // The primary name of the command (e.g. "copy")
    virtual std::string name() const = 0;

    // Alternative names / phrasings (e.g. {"duplicate", "clone"})
    virtual std::vector<std::string> aliases() const = 0;

    // One-line description shown in help listings
    virtual std::string description() const = 0;

    // Full usage string shown in detailed help
    virtual std::string syntax() const = 0;

    // Example usages shown in detailed help
    virtual std::vector<std::string> examples() const = 0;
};
