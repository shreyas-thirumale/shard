#pragma once

#include "../executor/Executor.h"

#include <memory>
#include <string>

class Shell {
public:
    Shell();
    ~Shell() = default;

    void run();

private:
    bool                    m_running;
    std::unique_ptr<Executor> m_executor;

    std::string readInput();
    void        processInput(const std::string& input);
    void        printPrompt() const;
    std::string suggestCommand(const std::string& input);
};
