#include "command_parser.h"

#include <cctype>
#include <sstream>

namespace {

std::string toUpper(std::string text) {
    for (char& ch : text) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }

    return text;
}

std::string trimLeadingSpace(const std::string& text) {
    std::size_t start = 0;

    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }

    return text.substr(start);
}

}  // namespace

ParsedCommand parseCommand(const std::string& line) {
    std::istringstream input(line);
    ParsedCommand parsed;

    input >> parsed.name;
    parsed.name = toUpper(parsed.name);

    if (parsed.name == "SET") {
        input >> parsed.key;
        std::getline(input, parsed.value);
        parsed.value = trimLeadingSpace(parsed.value);
    } else if (parsed.name == "GET" || parsed.name == "DEL" || parsed.name == "CACHE") {
        input >> parsed.key;
        if (parsed.name == "CACHE") {
            parsed.key = toUpper(parsed.key);
        }
        std::getline(input, parsed.value);
        parsed.value = trimLeadingSpace(parsed.value);
    }

    return parsed;
}

