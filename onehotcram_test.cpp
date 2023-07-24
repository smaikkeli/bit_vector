
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
  std::string test = "banana";
  if (argc == 2) {
    test = argv[1];
  }

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
  
  std::cout << "\nremoved the letter " << onehot.remove(0) << std::endl;
  onehot.set(0, 'a');

  for (size_t i = 0; i < onehot.size(); i++ ) {
    std::cout << onehot.at(i);
  }

  std::cout << "\nset index 0 to 'a'" << std::endl;

  for (size_t i = 0; i < onehot.size(); i++ ) {
    std::cout << onehot.at(i);
  }

  std::cout << "\nNumber of a's: " << onehot.rank('a') << std::endl;

  std::cout << "Position of the 3rd a in: " << onehot.select(3, 'a') << std::endl;

  return 0;
};
