#include <Cache.hpp>
#include <Disk.hpp>
#include <Strategy.hpp>

template<typename Strategy>
class KeyValueStore final {
 private:
  Strategy strategy;
  size_t size_max_cache;

 public:
  Cache cache;
  Disk disk;

  explicit KeyValueStore(size_t const bytes)
    : size_max_cache(bytes) {}
  ~KeyValueStore() = default;
  KeyValueStore(KeyValueStore const&) = delete;
  KeyValueStore(KeyValueStore &&) noexcept = delete;
  KeyValueStore &operator=(KeyValueStore const&) = delete;
  KeyValueStore &operator=(KeyValueStore &&) noexcept = delete;

  void record(string const& key, string const& value) {
    size_t const incoming_size = cache.incoming_size_change(key, value);
    while (cache.size() + incoming_size > size_max_cache) {
      strategy.onEviction(cache, disk);
    }

    if (cache.put(key, value)) {
      strategy.onRecord(key);
    } else {
      strategy.onAccess(key);
    }
  }

  optional<string> retrieve(string const& key) {
    optional<string> maybe_cache_value = cache.get(key);
    if (maybe_cache_value.has_value()) {
      strategy.onAccess(key);
      return maybe_cache_value;
    } else {
      optional<string> maybe_disk_value = disk.get(key);
      if (maybe_disk_value.has_value()) {
        disk.del(key);
        record(key, maybe_disk_value.value());
        return maybe_disk_value;
      } else {
        return nullopt;
      }
    }
  }

  bool del(string const& key) {
    if (cache.del(key)) {
      strategy.onDelete(key);
      return true;
    } else {
      return false;
    }
  }

  // used for interactive demonstration
  void delAll() {
    cache.delAll();
    strategy.delAll();
    disk.delAll();
  }
  // used for interactive demonstration
  void printAll() {
    cout << "Cache(" << cache.size() <<") contents:" << endl;
    cache.printAll();
    cout << "Strategy contents:" << endl;
    strategy.printAll();
    cout << "Disk contents:" << endl;
    disk.printAll();
  }
};
