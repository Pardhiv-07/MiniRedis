#include "cache.h"

LRUCache::LRUCache(std::size_t capacity) : capacity_(capacity) {
}

bool LRUCache::get(const std::string& key, std::string& value) {
    auto it = index_.find(key);

    if (it == index_.end()) {
        return false;
    }

    entries_.splice(entries_.begin(), entries_, it->second);
    value = it->second->second;
    return true;
}

void LRUCache::put(const std::string& key, const std::string& value) {
    if (capacity_ == 0) {
        return;
    }

    auto it = index_.find(key);

    if (it != index_.end()) {
        it->second->second = value;
        entries_.splice(entries_.begin(), entries_, it->second);
        return;
    }

    entries_.push_front({key, value});
    index_[key] = entries_.begin();

    if (index_.size() > capacity_) {
        evictLeastRecentlyUsed();
    }
}

bool LRUCache::remove(const std::string& key) {
    auto it = index_.find(key);

    if (it == index_.end()) {
        return false;
    }

    entries_.erase(it->second);
    index_.erase(it);
    return true;
}

void LRUCache::clear() {
    entries_.clear();
    index_.clear();
}

void LRUCache::setCapacity(std::size_t capacity) {
    capacity_ = capacity;

    while (index_.size() > capacity_) {
        evictLeastRecentlyUsed();
    }
}

std::size_t LRUCache::size() const {
    return index_.size();
}

std::size_t LRUCache::capacity() const {
    return capacity_;
}

std::vector<std::pair<std::string, std::string>> LRUCache::entriesMostRecentFirst() const {
    std::vector<std::pair<std::string, std::string>> entries;
    entries.reserve(index_.size());

    for (const auto& entry : entries_) {
        entries.push_back(entry);
    }
    return entries;
}

void LRUCache::evictLeastRecentlyUsed() {
    if (entries_.empty()) {
        return;
    }

    index_.erase(entries_.back().first);
    entries_.pop_back();
}
