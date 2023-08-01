#include <iostream>
#include <limits>
#include "./../bit_vector/bv.hpp"

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

template<typename dtype = char, class bit_vector = rle_bv>
class onehotcram{
  private: 
    size_t size_ {0};  //The size of onehotcram
    static const size_t alphabet_size_ {static_cast<size_t>(std::numeric_limits<dtype>::max()) + 1}; //The number of possible values for dtype
    uint32_t alphabet_bits[alphabet_size_]; 
    bit_vector bv;

  public:
    // @brief Default constructor, initializes the size to 0
    onehotcram() : alphabet_bits(), bv() {}

    // @brief Constructor, allows defining an initial size
    onehotcram(size_t input_size) : alphabet_bits(), bv(), size_(input_size) {
    
      for (size_t i = 0; i<alphabet_size_; i++) {
          for (size_t j = 0; j < input_size; j++) {
            bv.insert(0,0);
          }
          //Cumulative bits used for each index
          alphabet_bits[i] = i*input_size;
      }
    }

    size_t bv_size() const {
      return bv.size();
    }
    
    size_t size() const {
      return size_;
    }

    size_t alphabet_size() const {
      return alphabet_size_;
    }
    
    /*
     * @brief Retrieves the value of the index<sup>th</sup> element in the data 
     * structure.
     *
     * @return dtype value of the index<sup>th</sup> element.
     */
    dtype at(uint32_t index) {
      for (size_t i = 0; i < alphabet_size_; i++) {
        if (bv.at(alphabet_bits[i] + index)) {
          return static_cast<dtype>(i);
        }
      }
      return static_cast<dtype>(0); 
    }

    /*
     * @brief Insert "value" into position "index".
     *
     * @param index Where should `value` be inserted
     * @param value What should be inserted at `index`
    */
    void insert(uint32_t index, dtype elem) {
      size_++;
      //Loop through each element in the alphabet, add 0 or 1 
      bv.insert((alphabet_bits[0] + index), (0 == size_t(elem)));
         
      for (size_t i = 1; i < alphabet_size_; i++) {
        
        //compressed_size = bv[index < x < index + size_] << 

        alphabet_bits[i] = alphabet_bits[i-1] + size_;

        bv.insert((alphabet_bits[i] + index), (i == size_t(elem)));
      }
    }

    void remove(uint32_t index) {
      size_--;
      
      uint32_t current_bits = 0;

      bv.remove(alphabet_bits[0] + index);

      for (size_t i = 1; i < alphabet_size_; i++) {
        
        //Calculate number of bits required to store all information
        current_bits = size_;

        alphabet_bits[i] = alphabet_bits[i - 1] + current_bits;

        bv.remove(alphabet_bits[i] + index);
      }
    }

    void set(uint32_t index, dtype elem) {
      for (size_t i = 0; i < alphabet_size_; i++) {
        bv.set((alphabet_bits[i] + index), (i == size_t(elem)));
      }
    }
    
    uint32_t rank(dtype elem) {
      return bv.rank(alphabet_bits[size_t(elem)] + size_) - bv.rank(alphabet_bits[size_t(elem)]);
    }

    uint32_t select(uint32_t index, dtype elem) {
      return bv.select(rank(alphabet_bits[size_t(elem)]) + index) - alphabet_bits[size_t(elem)]; 
      
    }

};

