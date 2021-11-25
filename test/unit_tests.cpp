#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE KeyValueStore


#include <thread>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <KeyValueStore.hpp>


constexpr char alphanum[] = "0123456789"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz";
constexpr size_t alphanum_len = sizeof(alphanum)-1;

string random_string(size_t const len) {
  string ret{""};
  ret.reserve(len);

  for (size_t i = 0; i < len; ++i)
    ret += alphanum[rand() % alphanum_len];

  return ret;
}

BOOST_AUTO_TEST_SUITE(Tests)
  BOOST_AUTO_TEST_CASE(Test_Disk_PutGetDel) {
    Disk disk;
    // try to get non-existent element
    BOOST_CHECK_EQUAL(disk.get("111").has_value(), false);

    // check if "put" works
    disk.put("111", "aaa");
    BOOST_CHECK_EQUAL(disk.get("111").has_value(), true);
    BOOST_CHECK_EQUAL(disk.get("111").value(), "aaa");

    // try to delete a non-existent element
    BOOST_CHECK_EQUAL(disk.del("777"), false);

    // delete an existent element
    BOOST_CHECK_EQUAL(disk.del("111"), true);
    BOOST_CHECK_EQUAL(disk.get("111").has_value(), false);
  }

  BOOST_AUTO_TEST_CASE(Test_Cache_PutGetDel) {
    Cache cache;
    // try to get non-existent element
    BOOST_CHECK_EQUAL(cache.get("111").has_value(), false);

    // check if "put" works
    cache.put("111", "aaa");
    BOOST_CHECK_EQUAL(cache.get("111").has_value(), true);
    BOOST_CHECK_EQUAL(cache.get("111").value(), "aaa");

    // overwrite the element under existing key
    cache.put("111", "aa");
    BOOST_CHECK_EQUAL(cache.get("111").value(), "aa");

    // try to delete a non-existent element
    BOOST_CHECK_EQUAL(cache.del("777"), false);

    // delete an existent element
    BOOST_CHECK_EQUAL(cache.del("111"), true);
  }

  BOOST_AUTO_TEST_CASE(Test_CacheSize) {
    Cache cache;
    size_t number_of_records = 10,
           length_of_key = 3,
           length_of_value = 4;
    // fill the cash with random character strings
    for (size_t  i = 0; i < number_of_records; i++)
      cache.put(random_string(length_of_key), random_string(length_of_value));
    BOOST_CHECK_EQUAL(cache.size(), 70);
    // add additional element, size of cache increases by 6
    cache.put("111", "aaa");
    BOOST_CHECK_EQUAL(cache.size(), 76);
    // overwrite existing element with new value, size of cache reduce by 1
    cache.put("111", "aa");
    BOOST_CHECK_EQUAL(cache.size(), 75);
    // try to delete non-existent element, size of cache stays the same
    cache.del("777");
    BOOST_CHECK_EQUAL(cache.size(), 75);
    // delete existent element, size of cache reduces by 6
    cache.del("111");
    BOOST_CHECK_EQUAL(cache.size(), 70);
    // delete all elements, size of cache becomes 0
    cache.delAll();
    BOOST_CHECK_EQUAL(cache.size(), 0);
  }

  BOOST_AUTO_TEST_CASE(Test_KeyValueStore_FIFO) {
    KeyValueStore<FIFO> key_value_store(20);

    // prefill cache (6+6+6 characters)
    key_value_store.record("111", "aaa");
    key_value_store.record("222", "bbb");
    key_value_store.record("333", "ccc");
    key_value_store.record("4", "d");

    // FIFO behaviour

    // try to record beyond cache limit (18+6 characters > 20)
    // element "666":"evl" will go to cache
    /* element "111":"aaa" and "222":"bbb" will be evicted to disk according to
       FIFO behaviour*/
    key_value_store.record("666", "evil");
    BOOST_CHECK_EQUAL(key_value_store.cache.get("666").value(), "evil");

    // element "111":"aaa" and "222":"bbb" do not exist in cache (evicted)
    BOOST_CHECK_EQUAL(key_value_store.cache.get("111").has_value(), false);
    BOOST_CHECK_EQUAL(key_value_store.cache.get("222").has_value(), false);
    // check element "111":"aaa" and "222":"bbb" were migrated to disk
    BOOST_CHECK_EQUAL(key_value_store.disk.get("111").value(), "aaa");
    BOOST_CHECK_EQUAL(key_value_store.disk.get("222").value(), "bbb");
  }

  BOOST_AUTO_TEST_CASE(Test_KeyValueStore_LRU) {
    KeyValueStore<LRU> key_value_store(20);

    // prefill cache (6+6+6 characters)
    key_value_store.record("111", "aaa");
    key_value_store.record("222", "bbb");
    key_value_store.record("333", "ccc");
    key_value_store.record("4", "d");

    // LRU behaviour

    /* access first element "111":"aaa", therefore internal sequence
       for eviction will be updated according to LRU behaviour*/
    key_value_store.retrieve("111");

    // try to record beyond cache limit (18+6 characters > 20)
    // element "666":"evl" will go to cache
    /* element "222":"bbb" and "333":"ccc" will be evicted to disk according to
       updated LRU sequence*/
    key_value_store.record("666", "evil");
    BOOST_CHECK_EQUAL(key_value_store.cache.get("666").value(), "evil");

    // elements "222":"bbb" and "333":"ccc" do not exist in cache (evicted)
    BOOST_CHECK_EQUAL(key_value_store.cache.get("222").has_value(), false);
    BOOST_CHECK_EQUAL(key_value_store.cache.get("333").has_value(), false);
    // check element "222":"bbb" and "333":"ccc" were migrated to disk
    BOOST_CHECK_EQUAL(key_value_store.disk.get("222").value(), "bbb");
    BOOST_CHECK_EQUAL(key_value_store.disk.get("333").value(), "ccc");
  }

  BOOST_AUTO_TEST_CASE(Test_KeyValueStore_LFU) {
    KeyValueStore<LFU> key_value_store(20);

    // prefill cache (6+6+6 characters)
    key_value_store.record("111", "aaa");
    key_value_store.record("222", "bbb");
    key_value_store.record("333", "ccc");
    key_value_store.record("4", "d");

    // LFU behaviour

    // access element "111":"aaa" 2 times
    key_value_store.retrieve("111");
    key_value_store.retrieve("111");
    // access element "222":"bbb" 2 times
    key_value_store.retrieve("222");
    key_value_store.retrieve("222");
    // access element "333":"ccc" 1 time
    key_value_store.retrieve("333");

    // try to record beyond cache limit (18+6 characters > 20)
    // element "666":"evl" will go to cache
    /* element "333":"ccc" and "4":"d" will be evicted to disk,
       because they have the least amount of accesses*/
    key_value_store.record("666", "evil");
    BOOST_CHECK_EQUAL(key_value_store.cache.get("666").value(), "evil");

    // check element "333":"ccc" and "4":"d" does not exist in cache (evicted)
    BOOST_CHECK_EQUAL(key_value_store.cache.get("333").has_value(), false);
    BOOST_CHECK_EQUAL(key_value_store.cache.get("4").has_value(), false);
    // check element "222":"bbb"  and "4":"d" were migrated to disk
    BOOST_CHECK_EQUAL(key_value_store.disk.get("333").value(), "ccc");
    BOOST_CHECK_EQUAL(key_value_store.disk.get("4").value(), "d");
  }

  BOOST_AUTO_TEST_CASE(Test_KeyValueStore_PickingUpMissingKeysFromDisk) {
    KeyValueStore<FIFO> key_value_store(20);

    // record element "111":"aaa" into disk
    key_value_store.disk.put("111", "aaa");

    // check that cache is empty
    BOOST_CHECK_EQUAL(key_value_store.cache.size(), 0);

    // ask KeyValueStore for "111":"aaa"
    key_value_store.retrieve("111");

    // see that element "111":"aaa" was picked from disk to cache
    BOOST_CHECK_EQUAL(key_value_store.cache.get("111").has_value(), true);
    BOOST_CHECK_EQUAL(key_value_store.cache.get("111").value(), "aaa");
    // see that element "111":"aaa" does not exist in disk
    BOOST_CHECK_EQUAL(key_value_store.disk.get("111").has_value(), false);
  }

  BOOST_AUTO_TEST_CASE(Test_Threading) {
    size_t const number_of_threads = 3;
    vector<thread> threads;

    KeyValueStore<LFU> key_value_store(20);

    // simultaneous writing to KeyValueStore
    // thread 0 <- "000":"aaa", "111":"bbb"
    // thread 1 <- "111":"bbb", "222":"ccc"
    // thread 2 <- "222":"ccc", "333":"ddd"
    // record "000":"aaa" is evicted by "333":"ddd"
    /* records 111":"bbb" and "222":"ccc" are revisited by neighbouring threads,
       LFU algorithm increases frequency count,
       and therefore keeps them the cache*/
    for (size_t i=0; i < number_of_threads; i++) {
      threads.emplace_back([&key_value_store, id = i] () -> void {
        key_value_store.record(string(3, alphanum[id]),
                               string(3, alphanum[id+36]));
        key_value_store.record(string(3, alphanum[id+1]),
                               string(3, alphanum[id+1+36]));
      });
      if (threads[i].joinable())
        threads[i].join();
    }
    // key_value_store.printAll();
    // check eviction of record "000":"aaa"
    BOOST_CHECK_EQUAL(key_value_store.disk.get("000").value(), "aaa");
  }

BOOST_AUTO_TEST_SUITE_END()
