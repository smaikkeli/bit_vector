#include <iostream>
#include <limits>
#include "./../bit_vector/bv.hpp"

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

 //One hot cram one-hot encodes each of the characters in a given string
template<typename dtype = char, class bit_vector = rle_bv>
class onehotcram{
  private: 
    size_t size_ {0};  //The size of the container
    size_t alphabet_size_ {static_cast<size_t>(std::numeric_limits<dtype>::max()) + 1}; //The number of possible values for dtype
    uint64_t* alphabet_bits {new uint64_t [alphabet_size_]}; //Starting index for each character, alphabet_bits[0] = 0
    bit_vector* bv {new bit_vector}; //Pointer to the bitvector beneath the cumulative sums

  public:
    // @brief Default constructor, initializes the size to 0
    onehotcram() {}

    // @brief Constructor, allows defining an initial size
    onehotcram(size_t input_size) : size_(input_size) {
      uint64_t bv_size = input_size*alphabet_size_;
        
        //fill bitvector
        for (uint64_t i = 0; i < bv_size; i++) {
          bv->insert(0,0);
        } 
        
        //write number of bits used for each character
        for (uint64_t i = 0; i < alphabet_size_; i++) {
          alphabet_bits[i] = i*input_size;
        }
      }

    size_t bv_size() const {
      return bv->size();
    }
    
    size_t size() const {
      return size_;
    }

    size_t alphabet_size() const {
      return alphabet_size_;
    }

    dtype at(uint32_t index) {
      for (size_t i = 0; i < alphabet_size_; i++) {
        //std::cout << bv->at(alphabet_bits[i] + index) << std::endl;
        bool val = bv->at(alphabet_bits[i] + index);
        if (val) {
          return static_cast<dtype>(i);
        } 
      }
    }

    void insert(uint32_t index, dtype elem) {

      if (size_ == 0) {
        std::cout << "size is 0 " << std::endl;
        for (size_t i = 0; i < alphabet_size_; i++) {
          bv->insert(i, (i == size_t(elem)));
          if (i == size_t(elem)) {
            std::cout << " inserted element " << elem << " at bitvector index " << i << std::endl; 
          }
          alphabet_bits[i] = i;
          std::cout << "Starting bit for element " << static_cast<dtype>(i) << " is " << alphabet_bits[i] << std::endl;
        }
      } else { 
         //Loop through each element in the alphabet, add 0 or 1 
         for (size_t i = 0; i < alphabet_size_ - 1; i++) {
          if (i == size_t(elem)) {
           std::cout << i << " == " << size_t(elem) << std::endl;
           std::cout << alphabet_bits[i] + index  << " is the bitvector index" << std::endl;
          }
          bv->insert((alphabet_bits[i] + index), (i == size_t(elem)));
          alphabet_bits[i + 1]++;
          //Increment the next index, as insert(i) moves each j >= i by 1 
        }
      } 
      bv->insert(alphabet_bits[alphabet_size_-1], ((alphabet_size_ - 1) == size_t(elem)));
      size_++;
    }

    bool remove(uint64_t index) {
      for (size_t i = 0; i < alphabet_size_; i++) {
        uint32_t bv_index = alphabet_bits[i] + index;
        if (bv->at(bv_index)) {
             return bv->remove(bv_index);
            }
      }
      return false;
    }

    void set(uint64_t index, dtype elem) {
      for (size_t i = 0; i < alphabet_size_; i++) {
        uint32_t bv_index = alphabet_bits[i] + index;
        //Unset the old value
        if (bv->at(bv_index)) {
          bv->set((bv_index), false);
        }
        //Set the new value
        if (i == size_t(elem)) {
          bv->set((bv_index), true);
        }
      }
    }
    
    uint64_t rank(dtype elem) {
      return bv->rank(alphabet_bits[elem] + size_) - bv->rank(alphabet_bits[elem]);
    }

    uint64_t select(uint64_t index) {
      return 0;
    }

};


  
