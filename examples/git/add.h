#pragma once
#include <iostream>
#include <subparser/subparser.h>
#include <filesystem>

inline bool add(const subparser::Command& command) {
    namespace fs = std::filesystem;
    
    if (command.arguments.empty()) {
        std::cerr << "Nothing specified, nothing added\n";
        return true;
    }

    for (const auto& arg : command.arguments) {
        if (!fs::exists(arg)) {
            std::cerr << "fatal: path '" + arg + "' is not a file or directory\n";
            return false;
        }
    }

    return true;
}
