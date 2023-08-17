
#include <cstdint>
#include <sys/resource.h>

#include <cassert>
#include <string>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include "one_hot_cram/ohc.hpp"
#include "bit_vector/bv.hpp"

//> make ohc_test
//> ./ohc_test

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

int main (int argc, char *argv[]) {

  onehotcram<uint8_t, rle_bv, 1> onehot;
  for (size_t i = 0; i < 2000; i++) {
    onehot.insert(i, i%20); 
  }

  for (size_t i = 0; i< 1024; i++) {
    assert(onehot.at(i) == i%20);
  }

  for (size_t i = 0; i < 1024; i++) {
    onehot.set(i, 1);
  }

  for (size_t i = 0; i < 1024; i++) {
    assert(onehot.at(i) == 1);
  }
  std::cout << "alphabet size: " << onehot.alphabet_size() << std::endl;
  std::cout << "size of the datastructure: " << onehot.size() << std::endl;
  std::cout << "bitvector size: " << onehot.bv_size() << std::endl;
  return 0;
};
