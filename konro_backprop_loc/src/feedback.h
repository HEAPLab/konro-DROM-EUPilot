#pragma once
#include <random>

struct feedback {

  // Random seed
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_int_distribution<> dis;

  feedback() : gen(rd()), dis(-1, 1){};
  feedback(const feedback &) : gen(rd()), dis(-1, 1){};
  feedback(feedback &&) : gen(rd()), dis(-1, 1){};
  feedback &operator=(const feedback &) { return *this; }
  bool operator!=(const feedback &) { return true; }
  feedback &operator++() { return *this; }
  feedback &operator++(int) { return *this; }
  int operator*() { return dis(gen); }

  feedback &begin() { return *this; }

  feedback &end() { return *this; }
};