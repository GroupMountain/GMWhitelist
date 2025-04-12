#pragma once
#include <iostream>
namespace GMWhitelist {

struct Config {
    int version = 1;

    std::string language = "en_US";

    int CommandPermissionLevel = 4;

    bool ConsoleOutput = false;
};

} // namespace GMWhitelist