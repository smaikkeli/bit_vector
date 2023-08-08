#include <algorithm>
#include <iostream>
#include <limits>
#include "./../bit_vector/bv.hpp"

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

template<typename dtype = char, class bit_vector = rle_bv, int compression = 0>
class onehotcram{
  private: 
    /*The number of possible values in the alphabet*/
    static constexpr size_t ALPHABET_SIZE =   
      static_cast<size_t>(std::numeric_limits<dtype>::max()) + 1; 
    /*Size of alphabet_bits, 0 if not compressed*/
    
    /*Initialize alphabet bits if compressed, else to 1 to avoid compilation warnings*/
    uint32_t alphabet_bits[compression != 0 ? ALPHABET_SIZE : 1];

    size_t size_ = 0;  ///< Number of elements stored 
    bit_vector bv; ///<Bitvector where data is stored



  public:
    // @brief Default constructor
    onehotcram() : alphabet_bits(), bv() {}

    size_t bv_size() const {
      return bv.size();
    }
    
    size_t size() const {
      return size_;
    }

    size_t alphabet_size() const {
      return ALPHABET_SIZE;
    }
    
    /*
     * @brief Retrieves the value of the index<sup>th</sup> element in the data 
     * structure.
     *
     * @return dtype value of the index<sup>th</sup> element.
     */
    dtype at(uint32_t index) {
      if (compression == 1) {
        return at_c1(index);
      }

      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        if (bv.at(i*size_ + index)) {
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

      if (compression == 1) {
        return insert_c1(index, elem);
      }

      size_++;
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        bv.insert((i*size_ + index), (i == size_t(elem)));
      }
    }

    void remove(uint32_t index) {
      if (compression == 1) {
        return;
      }
      size_--;
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        bv.remove(size_*i + index);
      }
    }

    void set(uint32_t index, dtype elem) {
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        bv.set((size_*i + index), (i == size_t(elem)));
      }
    }
    
    uint32_t rank(dtype elem) {
      return bv.rank(size_t(elem)*size_ + size_) - bv.rank(size_t(elem)*size_);
    }

    uint32_t select(uint32_t index, dtype elem) {
      return bv.select(bv.rank(size_t(elem)*size_) + index) - size_t(elem)*size_; 
      
    }

    dtype at_c1(uint32_t index) {
      
      uint32_t index_size = 0; 
      uint32_t offset = 0;
      for (size_t i = 0; i < ALPHABET_SIZE - 1; i++) {
        index_size = alphabet_bits[i+1] - alphabet_bits[i];
        offset = size_ - index_size; 
        if (index >= offset) {
          if (bv.at(alphabet_bits[i] - offset + index)) {
            return static_cast<dtype>(i);
          }
        }
      }
      
      index_size = bv.size() - alphabet_bits[ALPHABET_SIZE - 1];
      offset = size_ - index_size;
      if (index >= offset) {
        if (bv.at(alphabet_bits[ALPHABET_SIZE - 1] - offset + index))
          return static_cast<dtype>(ALPHABET_SIZE - 1);
      } 
      return static_cast<dtype>(0);
    }
    
    void insert_c1(uint32_t index, dtype elem) {

      //Number of virtual leading zeros in bv
      uint32_t index_size = alphabet_bits[1] - alphabet_bits[0];
      uint32_t offset = size_ - index_size;
      uint32_t total_insertions = 0;

      if (index > offset) {
        bv.insert((index - offset), size_t(elem) == 0);
        total_insertions++;

      } else if (size_t(elem) == 0){

        uint32_t zero_insertions = offset - index;

        while (zero_insertions > 0) {
          bv.insert(0, 0);
          total_insertions++;
          zero_insertions--;
        }

        bv.insert(0, 1);
        total_insertions++;
      }
      
      alphabet_bits[1] += total_insertions;

      for (size_t i = 1; i < ALPHABET_SIZE - 1; i++) {
        
        index_size = int((alphabet_bits[i+1] + total_insertions) - alphabet_bits[i]) < 0 
          ? 0 
          : (alphabet_bits[i+1] + total_insertions) - alphabet_bits[i];

        offset = size_ - index_size; 

        if (index > offset) {
          bv.insert((alphabet_bits[i] + index - offset), size_t(elem) == i);
          total_insertions++;

        } else if (size_t(elem) == i) {

          uint32_t insertions = offset - index;

          while (insertions > 0) {
            bv.insert(alphabet_bits[i], 0);
            total_insertions++;
            insertions--;
          }

          bv.insert(alphabet_bits[i], 1);
          total_insertions++;
        }

        alphabet_bits[i+1] += total_insertions;
      }
       
      uint32_t last_elem = ALPHABET_SIZE - 1;
      index_size = int(bv.size() - alphabet_bits[last_elem]) < 0 
          ? 0 
          : (bv.size() - alphabet_bits[last_elem]);
      offset = size_ - index_size;

      if (index > offset) {
        bv.insert((alphabet_bits[last_elem] + index - offset), size_t(elem) == (last_elem));
        total_insertions++;
      } else if (size_t(elem) == last_elem) {

        uint32_t zero_insertions = offset - index;

        while (zero_insertions > 0) {
          bv.insert(alphabet_bits[last_elem], 0);
          total_insertions++;
          zero_insertions--;
        }

        bv.insert(alphabet_bits[last_elem], 1);
        total_insertions++;
      }
      
      size_++;
    }
};

