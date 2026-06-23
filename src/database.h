#ifndef MINIREDIS_DATABASE_H
#define MINIREDIS_DATABASE_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class Database {
public:
    void set(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;
    bool remove(const std::string& key);
    std::vector<std::string> keys() const;
    void forEachEntry(const std::function<void(const std::string&, const std::string&)>& visitor) const;
    std::size_t size() const;

private:
    std::unordered_map<std::string, std::string> store_;
};

#endif
