#include "ComparisonCounter.h"

unsigned long ComparisonCounter::counter = 0;

bool operator< (const ComparisonCounter& c, int v) {
  ComparisonCounter::counter++;
  return c.value < v;
}

bool operator< (int v, const ComparisonCounter& c) {
  ComparisonCounter::counter++;
  return v < c.value;
}

bool operator> (const ComparisonCounter& c, int v) {
  return !(c <= v);
}

bool operator== (const ComparisonCounter& c, int v) {
  ComparisonCounter::counter++;
  return c.value == v;
}

bool operator<= (const ComparisonCounter& c, int v) {
  ComparisonCounter::counter++;
  return c.value <= v;
}
