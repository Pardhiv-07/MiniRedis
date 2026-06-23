#include "database.h"

void Database::set(const std::string& key, const std::string& value) {
    store_[key] = value;
}

bool Database::get(const std::string& key, std::string& value) const {
    auto it = store_.find(key);

    if (it == store_.end()) {
        return false;
    }

    value = it->second;
    return true;
}

bool Database::remove(const std::string& key) {
    return store_.erase(key) > 0;
}

std::vector<std::string> Database::keys() const {
    std::vector<std::string> result;
    result.reserve(store_.size());

    for (const auto& entry : store_) {
        result.push_back(entry.first);
    }

    return result;
}

void Database::forEachEntry(const std::function<void(const std::string&, const std::string&)>& visitor) const {
    for (const auto& entry : store_) {
        visitor(entry.first, entry.second);
    }
}

std::size_t Database::size() const {
    return store_.size();
}
