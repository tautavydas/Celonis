#pragma once

#include <iostream>
#include <unordered_map>
#include <shared_mutex>

using namespace std;

class Cache final {
 private:
  unordered_map<string, string> table;
  size_t size_in_bytes{0};
  shared_mutex mutex;

 public:
  explicit Cache() = default;
  Cache(Cache const&) = delete;
  Cache(Cache &&) noexcept = delete;
  Cache &operator=(Cache const&) = delete;
  Cache &operator=(Cache &&) noexcept = delete;

  size_t size() {
    shared_lock<shared_mutex> read_lock(mutex);
    return size_in_bytes;
  }

  size_t incoming_size_change(string const& key, string const& value) {
    shared_lock<shared_mutex> read_lock(mutex);
    auto const& it = table.find(key);
    if (it == table.begin() || it == table.end()) {;
      return key.length() + value.length();
    } else {
      return value.length() - it->second.length();
    }
  }

  bool put(string const& key, string const& value) {
    unique_lock<shared_mutex> write_lock(mutex);
    auto const& [it, res] = table.try_emplace(key, value);
    if (res) {
      size_in_bytes += key.length() + value.length();
    } else {
      size_in_bytes += value.length() - it->second.length();
      it->second = value;
    }
    return res;
  }

  optional<string> get(string const& key) {
    shared_lock<shared_mutex> read_lock(mutex);
    auto const& it = table.find(key);
    if (it != table.end()) {
      return optional<string>{it->second};
    } else {
      return nullopt;
    }
  }

  bool del(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    auto const& it = table.find(key);
    if (it != table.end()) {
      size_in_bytes -= it->first.length() + it->second.length();
      table.erase(it);
      return true;
    }
    return false;
  }

  // used for interactive demonstration
  void delAll() {
    unique_lock<shared_mutex> write_lock(mutex);
    table.clear();
    size_in_bytes = 0;
  }

  // used for interactive demonstration
  void printAll() {
    shared_lock<shared_mutex> read_lock(mutex);
    if (table.empty()) {
      cout << "Cache is empty" << endl;
    } else {
      for (auto const& [key, val] : table) {
        cout << key << ':' << val << endl;
      }
    }
  }
};
