#include <algorithm>
#include <iostream>
#include <limits>
#include "./../bit_vector/bv.hpp"

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

template<typename dtype = char, class bit_vector = rle_bv, int compression = 0>
class onehotcram{
  private: 
    size_t size_ {0};  //The size of onehotcram
    static constexpr size_t alphabet_size_ 
      {static_cast<size_t>(std::numeric_limits<dtype>::max()) + 1}; //The number of possible values for dtype
    uint32_t alphabet_bits[alphabet_size_]; 
    bit_vector bv;

  public:
    // @brief Default constructor, initializes the size to 0
    onehotcram() : alphabet_bits(), bv() {}

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
      if (compression == 1) {
        return at_c1(index);
      }

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

      if (compression == 1) {
        return insert_c1(index, elem);
      }

      size_++;
      //Insert the first element to 0 + index;
      bv.insert(index, (0 == size_t(elem)));
         
      for (size_t i = 1; i < alphabet_size_; i++) {
        
        alphabet_bits[i] = alphabet_bits[i-1] + size_;

        bv.insert((alphabet_bits[i] + index), (i == size_t(elem)));
      }
    }

    void remove(uint32_t index) {
      if (compression == 1) {
        return remove_c1(index);
      }
      size_--;
      
      uint32_t current_bits = 0;

      bv.remove(alphabet_bits[0] + index);

      for (size_t i = 1; i < alphabet_size_; i++) {
        
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

    dtype at_c1(uint32_t index) {
      
      uint32_t index_size = 0; 
      uint32_t offset = 0;
      for (size_t i = 0; i < alphabet_size_ - 1; i++) {
        index_size = alphabet_bits[i+1] - alphabet_bits[i];
        offset = size_ - index_size; 
        if (index >= offset) {
          if (bv.at(alphabet_bits[i] - offset + index)) {
            return static_cast<dtype>(i);
          }
        }
      }
      
      index_size = bv.size() - alphabet_bits[alphabet_size_ - 1];
      offset = size_ - index_size;
      if (index >= offset) {
        if (bv.at(alphabet_bits[alphabet_size_ - 1] - offset + index))
          return static_cast<dtype>(alphabet_size_ - 1);
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

      for (size_t i = 1; i < alphabet_size_ - 1; i++) {
        
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
       
      uint32_t last_elem = alphabet_size_ - 1;
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
      
      /*for (size_t i = 0; i < 5; i++) {
        std::cout << i << ": " << alphabet_bits[i] << std::endl;
      }*/
      /*for (size_t i = 0; i < 5; i++) {
        std::cout << unsigned(i) << ": ";
        for (size_t j = alphabet_bits[i]; j < alphabet_bits[i+1]; j++) {
          std::cout << bv.at(j) << " ";
        } 
        std::cout << ", starting location: " << alphabet_bits[i] << std::endl;
      }*/
    }

    void remove_c1(uint32_t index) {
        void(0);
    }
};

