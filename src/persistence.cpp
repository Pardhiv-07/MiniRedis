#include "persistence.h"

#include <cstddef>
#include <fstream>
#include <string>

namespace {

std::string escapeField(const std::string& text) {
    std::string result;

    for (char ch : text) {
        if (ch == '\\') {
            result += "\\\\";
        } else if (ch == '\t') {
            result += "\\t";
        } else if (ch == '\n') {
            result += "\\n";
        } else if (ch == '\r') {
            result += "\\r";
        } else {
            result += ch;
        }
    }

    return result;
}

std::string unescapeField(const std::string& text) {
    std::string result;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] != '\\' || i + 1 >= text.size()) {
            result += text[i];
            continue;
        }

        char next = text[++i];
        if (next == '\\') {
            result += '\\';
        } else if (next == 't') {
            result += '\t';
        } else if (next == 'n') {
            result += '\n';
        } else if (next == 'r') {
            result += '\r';
        } else {
            result += next;
        }
    }

    return result;
}

}  // namespace

Persistence::Persistence(const std::string& filePath) : filePath_(filePath) {
}

PersistenceResult Persistence::save(const Database& db) const {
    std::ofstream file(filePath_);

    if (!file.is_open()) {
        return {false, "Could not open database file for writing"};
    }

    db.forEachEntry([&file](const std::string& key, const std::string& value) {
        file << escapeField(key) << '\t' << escapeField(value) << '\n';
    });

    if (!file.good()) {
        return {false, "Failed while writing database file"};
    }

    return {true, "Database Saved Successfully"};
}

PersistenceResult Persistence::load(Database& db) const {
    std::ifstream file(filePath_);

    if (!file.is_open()) {
        return {false, "Could not open database file for reading"};
    }

    Database loaded;
    std::string line;
    std::size_t skippedLines = 0;

    while (std::getline(file, line)) {
        std::size_t separator = line.find('\t');

        if (separator == std::string::npos) {
            ++skippedLines;
            continue;
        }

        std::string key = unescapeField(line.substr(0, separator));
        std::string value = unescapeField(line.substr(separator + 1));

        if (!key.empty()) {
            loaded.set(key, value);
        }
    }

    if (file.bad()) {
        return {false, "Failed while reading database file"};
    }

    db = loaded;

    if (skippedLines > 0) {
        return {true, "Database Loaded Successfully; skipped malformed lines"};
    }

    return {true, "Database Loaded Successfully"};
}
