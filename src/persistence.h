#ifndef MINIREDIS_PERSISTENCE_H
#define MINIREDIS_PERSISTENCE_H

#include "database.h"

#include <string>

struct PersistenceResult {
    bool success;
    std::string message;
};

class Persistence {
public:
    explicit Persistence(const std::string& filePath);

    PersistenceResult save(const Database& db) const;
    PersistenceResult load(Database& db) const;

private:
    std::string filePath_;
};

#endif
