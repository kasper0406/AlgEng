#include "test.h"

using namespace std;
using namespace std::chrono;

// It's very important to have a high quality random generator
// which has a long period such that the first part of the
// sequence doesn't match the last part exactly.
random_device rd;
mt19937 generator(rd());
uniform_int_distribution<int> distribution(numeric_limits<int>::min(), numeric_limits<int>::max());
auto random_int = bind(distribution, generator);

// Creates a random data of a specified length
vector<int> random_data(const size_t length) {
  vector<int> result(length);
  unordered_set<int> used;

  while (result.size() < length) {
    auto value = random_int();
    auto found = used.insert(value);
    if (!found.second) {
      result.push_back(value);
    }
  }

  return result;
};