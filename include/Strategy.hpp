#pragma once

#include <list>
#include <map>
#include <algorithm>
#include <shared_mutex>

using namespace std;

class FIFO final {
 private:
  list<string> fifo;
  shared_mutex mutex;

 public:
  FIFO() = default;
  ~FIFO() = default;
  FIFO(FIFO const&) = delete;
  FIFO(FIFO &&) noexcept = delete;
  FIFO &operator=(FIFO const&) = delete;
  FIFO &operator=(FIFO &&) noexcept = delete;

  void onRecord(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    fifo.emplace_front(key);
  }
  void onDelete(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    fifo.remove(key);
  }
  void onAccess(string const& key) const noexcept {}
  void onEviction(Cache & cache, Disk & disk) {
    unique_lock<shared_mutex> write_lock(mutex);
    disk.put(fifo.back(), cache.get(fifo.back()).value());
    cache.del(fifo.back());
    fifo.pop_back();
  }

  // used for interactive demonstration
  void printAll() {
    shared_lock<shared_mutex> read_lock(mutex);
    for (auto const& item : fifo)
      cout << item << endl;
  }
  // used for interactive demonstration
  void delAll() {
    unique_lock<shared_mutex> write_lock(mutex);
    fifo.clear();
  }
};

class LRU final {
 private:
  list<string> lru;
  shared_mutex mutex;

 public:
  LRU() = default;
  ~LRU() = default;
  LRU(LRU const&) = delete;
  LRU(LRU &&) noexcept = delete;
  LRU &operator=(LRU const&) = delete;
  LRU &operator=(LRU &&) noexcept = delete;

  void onRecord(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    lru.emplace_front(key);
  }
  void onDelete(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    lru.remove(key);
  }
  void onAccess(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    lru.splice(lru.begin(), lru, find(lru.begin(), lru.end(), key));
  }
  void onEviction(Cache & cache, Disk & disk) {
    unique_lock<shared_mutex> write_lock(mutex);
    disk.put(lru.back(), cache.get(lru.back()).value());
    cache.del(lru.back());
    lru.pop_back();
  }

  // used for interactive demonstration
  void printAll() {
    shared_lock<shared_mutex> read_lock(mutex);
    for (auto const& item : lru)
      cout << item << endl;
  }
  // used for interactive demonstration
  void delAll() {
    unique_lock<shared_mutex> write_lock(mutex);
    lru.clear();
  }
};

class LFU final {
 private:
  multimap<size_t, string> lfu;
  shared_mutex mutex;

 public:
  LFU() = default;
  ~LFU() = default;
  LFU(LFU const&) = delete;
  LFU(LFU &&) noexcept = delete;
  LFU &operator=(LFU const&) = delete;
  LFU &operator=(LFU &&) noexcept = delete;

  void onRecord(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    lfu.emplace(0, key);
  }
  void onDelete(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    lfu.erase(find_if(lfu.begin(), lfu.end(),
      [&key] (auto const& item) -> bool { return item.second == key; }));
  }
  void onAccess(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    auto const it = find_if(lfu.begin(), lfu.end(),
      [&key] (auto const& item) -> bool { return item.second == key; });
    auto const update = it->first+1;
    lfu.erase(it);
    lfu.emplace(update, key);
  }
  void onEviction(Cache & cache, Disk & disk) {
    unique_lock<shared_mutex> write_lock(mutex);
    disk.put(lfu.begin()->second, cache.get(lfu.begin()->second).value());
    cache.del(lfu.begin()->second);
    lfu.erase(lfu.begin());
  }

  // used for interactive demonstration
  void printAll() {
    shared_lock<shared_mutex> read_lock(mutex);
    if (lfu.empty()) {
      cout << "Strategy is empty" << endl;
    } else {
      for_each(lfu.rbegin(), lfu.rend(), [] (auto const& item) -> void {
        cout << item.first << ':' << item.second << endl;
      });
    }
  }
  // used for interactive demonstration
  void delAll() {
    unique_lock<shared_mutex> write_lock(mutex);
    lfu.clear();
  }
};
