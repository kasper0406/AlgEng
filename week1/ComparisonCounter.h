#pragma once

class ComparisonCounter {
public:
  static unsigned long counter;
    
  ComparisonCounter(int v) : value(v) { }

  bool operator<(const ComparisonCounter& other) {
    ComparisonCounter::counter++;
    return this->value < other.value;
  }
    
  bool operator==(const ComparisonCounter& other) {
    ComparisonCounter::counter++;
    return this->value == other.value;
  }

  friend bool operator< (ComparisonCounter& c, int v);
  friend bool operator< (int v, ComparisonCounter& c);
  friend bool operator> (ComparisonCounter& c, int v);
  friend bool operator== (ComparisonCounter& c, int v);
  friend bool operator<= (ComparisonCounter& c, int v);
    
private:
  int value;
};
