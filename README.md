# Celonis

## General

KeyValueStorage is a collection of numerous key-value pairs. It is a specific type of NoSQL database. The keys are unique identifiers for the values. The values can be any type of object -- a number or a string, or even another key-value pair in which case the structure of the database grows more complex.

## Key eviction srategies

**First-In-First-Out (FIFO)**

A double-linked list of keys is kept and being front-pushed and back-popped. Very similar to queue. In fact we could use queue, if we didn't have to remove elements from the middle.

**Least Recently Used (LRU)**

A double-linked list of keys is kept and being front-pushed and back-popped. During every access the element IS put to front of the list (relinked). Therefore least used elements appear in the end of the list and therefore evicted eventually.

**Least Frequently Used (LFU)**

An ordered-multimap of counters-as-keys and keys-as-values is recorded. The goal is to track a number of accesses of the original  keys, therefore a counter for each key. Since keys of the "queue" are integers, we must allow non-unique entries. And they must be ordered, so that we always evict the element, which has the key-access-count the lowest (the first element). Upon access we find the counter represeting the key, increase the counter by 1 and put the entry back to the "queue". Upon reentry the key climbs the ordered map and avoids eviction. The higher the counter, the less chance for eviction. Deletion always happens upon request, no matter how high the counter.

https://stackoverflow.com/questions/1436020/whats-the-difference-between-deque-and-list-stl-containers
https://www.fluentcpp.com/2018/12/11/overview-of-std-map-insertion-emplacement-methods-in-cpp17/
https://github.com/vpetrigo/caches/blob/master/include/lfu_cache_policy.hpp
