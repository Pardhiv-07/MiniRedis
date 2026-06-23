#include "cache.h"
#include "command_parser.h"
#include "database.h"
#include "persistence.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void handleSet(Database& db, LRUCache& cache, const ParsedCommand& command) {
    if (command.key.empty() || command.value.empty()) {
        std::cout << "Usage: SET <key> <value>\n";
        return;
    }

    db.set(command.key, command.value);
    cache.put(command.key, command.value);
    std::cout << "OK\n";
}

void handleGet(const Database& db, LRUCache& cache, const ParsedCommand& command) {
    if (command.key.empty()) {
        std::cout << "Usage: GET <key>\n";
        return;
    }

    std::string value;
    if (cache.get(command.key, value)) {
        std::cout << value << "\n";
    } else if (db.get(command.key, value)) {
        cache.put(command.key, value);
        std::cout << value << "\n";
    } else {
        std::cout << "(nil)\n";
    }
}

void handleDel(Database& db, LRUCache& cache, const ParsedCommand& command) {
    if (command.key.empty()) {
        std::cout << "Usage: DEL <key>\n";
        return;
    }

    bool deleted = db.remove(command.key);
    cache.remove(command.key);
    std::cout << (deleted ? "OK" : "Key not found") << "\n";
}

void handleSave(const Database& db, const Persistence& persistence) {
    PersistenceResult result = persistence.save(db);
    if (result.success) {
        std::cout << result.message << "\n";
    } else {
        std::cout << "Error: " << result.message << "\n";
    }
}

bool handleLoad(Database& db, const Persistence& persistence) {
    PersistenceResult result = persistence.load(db);
    if (result.success) {
        std::cout << result.message << "\n";
        return true;
    }

    std::cout << "Error: " << result.message << "\n";
    return false;
}

void printCacheVisualizer(const LRUCache& cache) {
    auto entries = cache.entriesMostRecentFirst();

    std::cout << "LRU Cache Visualizer\n";
    std::cout << "Capacity: " << cache.capacity() << ", Size: " << cache.size() << "\n\n";

    if (entries.empty()) {
        std::cout << "[MRU] (empty) [LRU]\n";
        return;
    }

    std::cout << "[MRU] ";
    for (std::size_t i = 0; i < cache.capacity(); ++i) {
        if (i < entries.size()) {
            std::cout << "[" << entries[i].first << ":" << entries[i].second << "]";
        } else {
            std::cout << "[empty]";
        }

        if (i + 1 < cache.capacity()) {
            std::cout << " <-> ";
        }
    }
    std::cout << " [LRU]\n\n";

    std::cout << "Slot  Recency  Key  Value\n";
    for (std::size_t i = 0; i < entries.size(); ++i) {
        std::string recency = "middle";
        if (i == 0) {
            recency = "MRU";
        } else if (i + 1 == entries.size()) {
            recency = "LRU";
        }

        std::cout << std::setw(4) << i + 1 << "  "
                  << std::setw(7) << recency << "  "
                  << entries[i].first << "  "
                  << entries[i].second << "\n";
    }
}

void handleCache(LRUCache& cache, const ParsedCommand& command) {
    if (command.key == "SIZE") {
        std::istringstream input(command.value);
        std::size_t capacity = 0;
        input >> capacity;

        if (!input || capacity == 0) {
            std::cout << "Usage: CACHE SIZE <positive-number>\n";
            return;
        }

        cache.setCapacity(capacity);
        cache.clear();
        std::cout << "Cache capacity set to " << capacity << "\n";
        return;
    }

    if (command.key == "VISUAL") {
        printCacheVisualizer(cache);
        return;
    }

    auto entries = cache.entriesMostRecentFirst();

    std::cout << "Cache capacity: " << cache.capacity() << "\n";

    if (entries.empty()) {
        std::cout << "(empty)\n";
        return;
    }

    for (const auto& entry : entries) {
        std::cout << entry.first << " => " << entry.second << "\n";
    }
}

void printHelp() {
    std::cout << "Commands available in Phase 6:\n";
    std::cout << "  SET <key> <value>\n";
    std::cout << "  GET <key>\n";
    std::cout << "  DEL <key>\n";
    std::cout << "  KEYS\n";
    std::cout << "  SAVE\n";
    std::cout << "  LOAD\n";
    std::cout << "  CACHE\n";
    std::cout << "  CACHE SIZE <capacity>\n";
    std::cout << "  CACHE VISUAL\n";
    std::cout << "  HELP\n";
    std::cout << "  EXIT\n";
}

}  // namespace

int main() {
    Database db;
    LRUCache cache(3);
    Persistence persistence("data/database.txt");
    std::string line;

    persistence.load(db);

    std::cout << "MiniRedis Phase 6 - Integrated database, persistence, and cache\n";
    printHelp();

    while (true) {
        std::cout << "\nMiniRedis > ";

        if (!std::getline(std::cin, line)) {
            break;
        }

        ParsedCommand command = parseCommand(line);

        if (command.name == "SET") {
            handleSet(db, cache, command);
        } else if (command.name == "GET") {
            handleGet(db, cache, command);
        } else if (command.name == "DEL") {
            handleDel(db, cache, command);
        } else if (command.name == "KEYS") {
            auto keys = db.keys();

            if (keys.empty()) {
                std::cout << "(empty)\n";
                continue;
            }

            for (const auto& key : keys) {
                std::cout << key << "\n";
            }
        } else if (command.name == "SAVE") {
            handleSave(db, persistence);
        } else if (command.name == "LOAD") {
            if (handleLoad(db, persistence)) {
                cache.clear();
            }
        } else if (command.name == "CACHE") {
            handleCache(cache, command);
        } else if (command.name == "HELP") {
            printHelp();
        } else if (command.name == "EXIT") {
            std::cout << "Goodbye\n";
            break;
        } else if (!command.name.empty()) {
            std::cout << "Unknown command. Type HELP for available commands.\n";
        }
    }

    return 0;
}
