
#include <sys/resource.h>

#include <cassert>
#include <string>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include "one_hot_cram/ohc.hpp"
#include "bit_vector/bv.hpp"

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

int main (int argc, char *argv[]) {
  onehotcram<char> onehot;
  std::string test = "banana";

  for(size_t i = 0; i<test.size(); i++) {
        onehot.insert(i, test[i]);
    }
  std::cout << "alphabet size: " << onehot.alphabet_size() << std::endl;
  std::cout << "size of the datastructure: " << onehot.size() << std::endl;
  std::cout << "bitvector size: " << onehot.bv_size() << std::endl;
  for (size_t i = 0; i<test.size(); i++ ) {
    std::cout << onehot.at(i) << std::endl;
  }

  return 0;
};
