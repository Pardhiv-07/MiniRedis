#ifndef MINIREDIS_CACHE_H
#define MINIREDIS_CACHE_H

#include <cstddef>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class LRUCache {
public:
    explicit LRUCache(std::size_t capacity);

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
    LRUCache(LRUCache&&) noexcept = default;
    LRUCache& operator=(LRUCache&&) noexcept = default;

    bool get(const std::string& key, std::string& value);
    void put(const std::string& key, const std::string& value);
    bool remove(const std::string& key);
    void clear();
    void setCapacity(std::size_t capacity);

    std::size_t size() const;
    std::size_t capacity() const;
    std::vector<std::pair<std::string, std::string>> entriesMostRecentFirst() const;

private:
    using CacheEntry = std::pair<std::string, std::string>;
    using EntryList = std::list<CacheEntry>;
    using EntryIterator = EntryList::iterator;

    void evictLeastRecentlyUsed();

    std::size_t capacity_;
    EntryList entries_;
    std::unordered_map<std::string, EntryIterator> index_;
};

#endif
