#include "threadsafe_map.hpp"

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <chrono>
#include <exception>

void insert_numbers(const int start, ThreadsafeMap<int, std::string> &safe_map) {
  for(int i = start; i < start + 100; ++i) {
    safe_map.insert_or_update(i, std::to_string(i));
  }
}

void erase_numbers(const int start, ThreadsafeMap<int, std::string> &safe_map) {
  try {
    for(int i = start; i < start + 100; ++i) {
      safe_map.erase(i);
    }
  }
  catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}




int main() {
  ThreadsafeMap<int, std::string> safe_map;

  std::vector<std::thread> threads;

  std::map<int, std::string> halfway_map, final_map;

  int num_threads = 16;
  
  for(int i = 0; i < num_threads; ++i) {
    threads.push_back(std::thread(insert_numbers, i * 100, std::ref(safe_map)));
  }

  halfway_map = safe_map.get_map();

  for(int i = 0; i < num_threads; ++i) {
    threads[i].join();
  }

  std::cout << "\n\nHALFWAY MAP AFTER ADDING SOME NUMBERS:" << std::endl;
  for(const auto &it: halfway_map)
    std::cout << "{" << it.first << ", " << it.second << "}, ";
  std::cout << "\n";

  final_map = safe_map.get_map();
  std::cout << "\n\nFINAL MAP AFTER ADDING ALL NUMBERS:" << std::endl;
  for(const auto &it: final_map)
    std::cout << "{" << it.first << ", " << it.second << "}, ";
  std::cout << "\n";

  threads = std::vector<std::thread>();

  for(int i = 0; i < num_threads; ++i) {
    threads.push_back(std::thread(erase_numbers, i * 50, std::ref(safe_map)));
  }  

  for(int i = 0; i < num_threads; ++i) {
    threads[i].join();
  }

  std::cout << "\n\nFINAL MAP AFTER ERASING SOME NUMBERS:" << std::endl;
  final_map = safe_map.get_map();
  for(const auto &it: final_map)
    std::cout << "{" << it.first << ", " << it.second << "}, ";
  std::cout << "\n";

  std::cout << "\n\nTRYING TO GET ALL THE NUMBERS FROM 0 TO NUM_THREADS * 100" << std::endl;
  for(int i = 0; i < num_threads * 100; ++i) {
    auto val = safe_map.get(i, "NOT_FOUND");
    std::cout << "{" << val << "}, ";
  }
  std::cout << "\n";


  return 0;
}