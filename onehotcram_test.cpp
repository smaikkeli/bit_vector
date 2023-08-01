
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

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

int main (int argc, char *argv[]) {
  std::string test = "persereika";

  onehotcram<char, bv::bv> onehot;

  
  for (size_t i = 0; i<test.size(); i++) {
    onehot.insert(i, test[i]);
  }
  std::cout << "alphabet size: " << onehot.alphabet_size() << std::endl;
  std::cout << "size of the datastructure: " << onehot.size() << std::endl;
  std::cout << "bitvector size: " << onehot.bv_size() << std::endl;
  for (size_t i = 0; i < onehot.size(); i++ ) {
    std::cout << onehot.at(i);
  }
  
  onehot.remove(0);
  std::cout << "\nremoved the letter at index 0" << std::endl;

  for (size_t i = 0; i < onehot.size(); i++ ) {
    std::cout << onehot.at(i);
  }

  onehot.set(0, 'p');
  std::cout << "\nset index 0 to 5" << std::endl;

  for (size_t i = 0; i < onehot.size(); i++ ) {
    std::cout << onehot.at(i);
  }

  std::cout << "\nNumber of 3's: " << onehot.rank('e') << std::endl;

  std::cout << "Position of the 2nd 3 in: " << onehot.select(2, 'e') << std::endl;

  return 0;
};
