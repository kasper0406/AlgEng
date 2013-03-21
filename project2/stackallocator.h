#pragma once

#include <cstdint>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>

using namespace std;

class StackAllocator {
private:
  typedef map<uint8_t*, bool, greater<uint8_t*>> free_map; // Reversed

  uint8_t* data;
  uint8_t* current;
  free_map freed;
  size_t prealloc_size_in_bytes;
  
public:
  StackAllocator(size_t prealloc_size_in_bytes) {
    this->prealloc_size_in_bytes = prealloc_size_in_bytes;

#ifndef _WINDOWS
    int res = posix_memalign((void**)&data, CACHE_LINE_SIZE, prealloc_size_in_bytes);
    if (res != 0)
      throw runtime_error("Could not allocate memory!");
#else
    data = new uint8_t[prealloc_size_in_bytes];
#endif
    current = data;
  };

  template <typename T>
  T* alloc(size_t elements) {
    uint8_t* result = current;

    if (((size_t)current + (size_t)sizeof(T) * (size_t)elements) > (size_t)data + prealloc_size_in_bytes) {
      print();
      throw logic_error(string("Stack allocator: Out of memory. Tried to allocate ") + to_string(sizeof(T) * elements) + " bytes.");
    }

    current += sizeof(T) * elements;

    freed[result] = false;

    return (T*)result;
  };

  template <typename T>
  void free(T* ptr) {
    freed[(uint8_t*)ptr] = true;

    for (free_map::const_iterator iter = freed.begin(); iter != freed.end(); /* inc done with erase */) {
      if (iter->second == false)
        break;

      current = iter->first;

      freed.erase(iter);
      iter = freed.begin();
    }
  };

  void print() const {
    cout << "Data ptr: " << (int*)data << endl;
    cout << "Current ptr: " << (int*)current << endl;
    cout << "Pre allocated: " << prealloc_size_in_bytes << " bytes" << endl;
    cout << "Bytes available: " << (size_t)((data + prealloc_size_in_bytes) - current) << " bytes" << endl;
    cout << "Free map:" << endl;
    for (free_map::const_iterator iter = freed.begin(); iter != freed.end(); iter++) {
      cout << "  Key: " << (int*)iter->first << " Freed: " << iter->second << endl;
    }
    cout << endl;
  };

  ~StackAllocator() {
#ifndef _WINDOWS
        free(data);
#else
        delete[] data;
#endif
  };
};