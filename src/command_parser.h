#ifndef MINIREDIS_COMMAND_PARSER_H
#define MINIREDIS_COMMAND_PARSER_H

#include <string>

struct ParsedCommand {
    std::string name;
    std::string key;
    std::string value;
};

ParsedCommand parseCommand(const std::string& line);

#endif

