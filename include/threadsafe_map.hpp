#ifndef ASYNCHRONOUS_SERVER_THREADSAFE_MAP_HPP
#define ASYNCHRONOUS_SERVER_THREADSAFE_MAP_HPP

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <shared_mutex>
#include <utility>
#include <vector>

#include <iostream>
#include <thread>

template <typename Key, typename Value>
class ThreadsafeBucket;

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class ThreadsafeMap;

template <typename Key, typename Value>
class ThreadsafeBucket {
public:
  template <typename K, typename V, typename H>
  friend class ThreadsafeMap;

  ThreadsafeBucket() {}

  // insert
  void insert_value_for(const Key& key, const Value& value) {
    std::unique_lock lk(mutx_);

    auto it = find_entry_for(key);

    if(it != data_.end())
      it->second = value;
    else
      data_.push_back({key, value});
  }

  // delete
  void erase_value_for(const Key& key) {
    std::unique_lock<std::shared_mutex> lk(mutx_);

    auto it = find_entry_for(key);
    if(it != data_.end())
      data_.erase(it);

    return;
  }

  /**
   * @brief Gets the value for a certain key. If that value cannot be found,
   * return a default object of Value type. UNDERSTAND WHY THIS IS NEEDED.
   * 
   * @param key 
   * @param default_value 
   * @return Value 
   */
  Value get_value_for(const Key& key, const Value& default_value=Value()) const {
    std::shared_lock<std::shared_mutex> lk(mutx_);
    auto it = find_entry_for(key);
    
    return (it != data_.end()) ? it->second : default_value;
  }

private:
  
  /**
   * @brief Finds the entry with a certain key in the list. Returns data.end() if not
   * found. Does not need to lock the mutex since it is only called in functions where
   * the mutex is already locked.
   * 
   * @param key 
   * @return std::list<std::pair<Key, T>>::iterator 
   */
  typename std::list<std::pair<Key, Value>>::iterator find_entry_for(const Key& key) {
    auto it = data_.begin();
    
    for(; it != data_.end(); ++it) {
      if(it->first == key)
        return it;
    }

    return it;
  }

  typename std::list<std::pair<Key, Value>>::const_iterator find_entry_for(const Key& key) const {
    auto it = data_.cbegin();
    
    for(; it != data_.cend(); ++it) {
      if(it->first == key)
        return it;
    }

    return it;
  }

private:
  std::list<std::pair<Key, Value>> data_;
  mutable std::shared_mutex mutx_;
};

/**
 * @brief Threadsafe map. Provides thread-safe access, insertion, and removal operations,
 * all on constant time, on average. Supports multiple reader threads but a single writer
 * thread. Does so by using independent buckets that have their own independent mutex.  
 * 
 * Provides a thread-safe way to get a snapshot of the map at a certain point.
 * Changes after taking the snapshot are not seen in it. Based on the example in
 * chapter 6 of Anthony Williams' C++ Concurrency in Action.
 * 
 * Fixed size amount of buckets means that the amount must be chosen at the time of 
 * instatiation. It is advised to choose prime numbers for the size as the hashing
 * functions perform better for said sizes.
 * 
 * @tparam Key 
 * @tparam T 
 */
template <typename Key, typename Value, typename Hash>
class ThreadsafeMap {
public:
  ThreadsafeMap(const unsigned int number_buckets = 19, const Hash& hasher=Hash())
  // : buckets_(number_buckets, std::make_unique<ThreadsafeBucket<Key,Value>>()),
  : hasher_(hasher) { 
    for(int i = 0; i < number_buckets; ++i)
      buckets_.push_back(
        std::unique_ptr<ThreadsafeBucket<Key, Value>>(
          new ThreadsafeBucket<Key, Value>()
        )
      );
  }

  // Also, understand better why this is.
  ThreadsafeMap(const ThreadsafeMap& other) = delete;
  ThreadsafeMap& operator=(const ThreadsafeMap &other) = delete;

  // insert
  void insert_or_update(const Key& key, const Value& value) {
    get_bucket(key).insert_value_for(key, value);
    return;
  }

  // erase
  void erase(const Key& key) {
    get_bucket(key).erase_value_for(key);
    return;
  }

  // get
  Value get(const Key& key, const Value& default_value=Value()) const{
    return get_bucket(key).get_value_for(key, default_value);
  }

  // snapshot
  std::map<Key, Value> get_map() const {
    // lock the buckets
    std::vector<std::shared_lock<std::shared_mutex>> locks;

    for(auto &bucket : buckets_)
      locks.push_back(std::shared_lock<std::shared_mutex>(bucket->mutx_));

    std::map<Key, Value> result;
    for(const auto &bucket : buckets_)
      for(const auto &entry: bucket->data_)
        result.insert(entry);

    return result;
  }


private:
  const ThreadsafeBucket<Key, Value>& get_bucket(const Key& key) const {
    typename std::vector<ThreadsafeBucket<Key, Value>>::size_type idx;
    idx = hasher_(key) % buckets_.size();

    return *buckets_[idx];
  }

  ThreadsafeBucket<Key, Value>& get_bucket(const Key& key) {
    typename std::vector<ThreadsafeBucket<Key, Value>>::size_type idx;
    idx = hasher_(key) % buckets_.size();
    
    return *buckets_[idx];
  }


private:
  std::vector<std::unique_ptr<ThreadsafeBucket<Key, Value>>> buckets_;
  Hash hasher_;

}; 

#endif // ASYNCHRONOUS_SERVER_THREADSAFE_MAP_HPP
