#pragma once

class ComparisonCounter {
public:
  static unsigned long counter;
    
  ComparisonCounter(int v) : value(v) { }

  bool operator<(const ComparisonCounter& other) const {
    ComparisonCounter::counter++;
    return this->value < other.value;
  }
    
  bool operator==(const ComparisonCounter& other) const {
    ComparisonCounter::counter++;
    return this->value == other.value;
  }

  friend bool operator< (const ComparisonCounter& c, int v);
  friend bool operator< (int v, const ComparisonCounter& c);
  friend bool operator> (const ComparisonCounter& c, int v);
  friend bool operator== (const ComparisonCounter& c, int v);
  friend bool operator<= (const ComparisonCounter& c, int v);
    
private:
  int value;
};
