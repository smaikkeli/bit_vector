
#include <sys/resource.h>

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include "one_hot_cram/ohc.hpp"
#include "bit_vector/bv.hpp"

int main (int argc, char *argv[]) {
  onehotcram onehot;
  char a = 65;
  onehot.insert(0, a);
  std::cout << onehot.at(0) << std::endl;

  return 0;
};
