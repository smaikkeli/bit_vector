
#include <sys/resource.h>

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include "one_hot_cram/ohc.hpp"
#include "bit_vector/bv.hpp"

int main (int argc, char *argv[]) {
  onehotcram<char, bv::simple_bv<16, 16384, 64, true, true>> onehot(256);
  std::cout << onehot.size() << std::endl;

  return 0;
};
