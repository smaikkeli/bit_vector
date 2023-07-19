#include <iostream>
#include <vector>
#include "./../bit_vector/bv.hpp"

 //One hot cram one-hot encodes each of the characters in a given string
template<class dtype = char, class bit_vector = bv::bv>
class onehotcram{
  private: 
    size_t size_;  //The number of elements inserted
    size_t alphabet_size;
    uint64_t* alphabet; //alphabet, includes the number of bits used for each symbol
    bit_vector* bv; //Pointer to the bitvector beneath the cumulative sums

  public:
    onehotcram() : 
      size_(0),  
      alphabet_size((1UL << sizeof(dtype)) - 1), //Alphabet size is the maximum integer i.e. char = 255
      alphabet(new uint64_t [(1UL << sizeof(dtype)) - 1]), 
      bv(new bit_vector) {}

    size_t size() const {
      return size_;
    }
    
    dtype at(uint64_t index) {
      for (size_t i = 0; i < alphabet_size; i++) {
        bool val = bv->at(alphabet[i] + index);
        if (val) {
          std::cout << "found" << std::endl;
          return static_cast<dtype>(i);
        } 
      }
    }

    void insert(uint64_t index, dtype elem) {
      
      if (index > size_) {
        std::cout << "index " << index << " out of range" << std::endl;
        return;
      }
      //Initialize new 0's for the insertion, then set!!!!!! 
      if (size_ == 0) {
        std::cout << "started initialization " << std::endl;
        for (size_t i = 0; i < alphabet_size; i++) {
          bv->insert(0, 0);
          std::cout << "insert" << std::endl;
          if (uint64_t(i) == uint64_t(elem)) {
            bv->set((alphabet[i] + index), 1);
            std::cout << "set" << std::endl;
            size_++;
          }
        }
        return;
      }

      bv->insert(index, (0UL == uint64_t(elem)));
      //Naive implementation, add one bit for each element in the alphabet
      for (size_t i = 1; i < alphabet_size; i++) {
        alphabet[i] = alphabet[i-1] + 1; 
        bv->insert((alphabet[i] + index), (uint64_t(i) == uint64_t(elem)));
        std::cout << "inserted at " << i << std::endl;
      }
      size_++;
    }

    bool remove(uint64_t index) {
      return false;
    }

    bool set(uint64_t index, dtype elem) {
      return false;
    }

    uint64_t rank(uint64_t index) {
      return 0;
    }

    uint64_t select(uint64_t index) {
      return 0;
    }

};


  
