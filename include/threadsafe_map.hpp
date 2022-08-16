#ifndef ASYNCHRONOUS_SERVER_THREADSAFE_MAP_HPP
#define ASYNCHRONOUS_SERVER_THREADSAFE_MAP_HPP

#include <list>
#include <map>
// #include <mutex>
#include <shared_mutex>
#include <utility>

template <typename Key, typename Value>
class ThreadsafeBucket {
public:
  ThreadsafeBucket() {}

  // insert

  // delete

  /**
   * @brief Gets the value for a certain key. If that value cannot be found,
   * return a default object of Value type. UNDERSTAND WHY THIS IS NEEDED.
   * 
   * @param key 
   * @param default_value 
   * @return Value 
   */
  Value get_value_for(const Key &key, const Value &default_value=Value()) {
    std::shared_lock<std::shared_mutex> lk(mutx);
    auto it = find_entry_for(key);
    
    return (it != data.end()) ? it->second : default_value;
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
  typename std::list<std::pair<Key, Value>>::iterator find_entry_for(const Key &key) const {
    auto it = data.begin();
    
    for(; it != data.end(); ++it) {
      if(it->first == key)
        return it;
    }

    return it;
  }

private:
  std::list<std::pair<Key, Value>> data;
  mutable std::shared_mutex mutx;
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
 * @tparam Key 
 * @tparam T 
 */
template <typename Key, typename Value>
class ThreadsafeMap {
private:
  std::mutex mut;
  // std::condition_variable/
  std::map<Key, Value> data_map;

public:
  ThreadsafeMap() {}

}; 

#endif // ASYNCHRONOUS_SERVER_THREADSAFE_MAP_HPP
