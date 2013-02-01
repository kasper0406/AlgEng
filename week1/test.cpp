#include "test.h"

using namespace std;
using namespace std::chrono;

// It's very important to have a high quality random generator
// which has a long period such that the first part of the
// sequence doesn't match the last part exactly.
random_device rd;
mt19937 generator(rd());
uniform_int_distribution<int> distribution(numeric_limits<int>::min(), numeric_limits<int>::max() - 1); // Max is used for "no element"
auto random_int = bind(distribution, generator);

// Creates a random data of a specified length
vector<int> unique_random_data(const size_t length) {
  vector<int> result;
  unordered_set<int> used;

  while (result.size() < length) {
    auto value = random_int();
    auto found = used.insert(value);
    if (found.second) {
      result.push_back(value);
    }
  }

  return result;
};

vector<int> random_data(const size_t length) {
    vector<int> result;
    for (int i = 0; i < length; i++)
        result.push_back(random_int());
    return result;
}

int result_dist(vector<pair<bool,int>> a, vector<pair<bool,int>> b) {
    if (a.size() != b.size())
        throw runtime_error("Invalid results!");
    
    int dist = 0;
    for (int i = 0; i < a.size(); i++) {
        if (a[i].first != b[i].first ||
            (a[i].first == true && a[i].second != b[i].second))
            dist++;
    }
    return dist;
}
