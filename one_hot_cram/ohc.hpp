#include <iostream>
#include <vector>
#include "./../bit_vector/bv.hpp"
#include "./../bit_vector/internal/allocator.hpp"

 //One hot cram one-hot encodes each of the characters in a given string
template<class dtype, class bit_vector = bv::bv> 
class onehotcram{
  private: 
    uint64_t alphabet_size_; //The size of the alphabet
    uint64_t* data; //Data, includes the number of bits used for each symbol
    //alloc* a; //Allocator for bitvector
    bit_vector* bitvector; //Pointer to the bitvector beneath the cumulative sums

  public:
    //What to put for the constructor??
    onehotcram(uint64_t alph_size) {
      alphabet_size_ = alph_size;
      data = new uint64_t [sizeof(dtype)^2 + 1];
    }

    size_t size() {
      return sizeof(data);
    }

    void insert(uint64_t index, dtype elem) {
      void(0);
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


  
