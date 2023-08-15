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
    
    /*Initialize alphabet bits if compressed, else to 1 to avoid compilation warnings*/
    uint32_t alphabet_bits[compression != 0 ? ALPHABET_SIZE : 1];

    size_t size_ = 0;  ///< Number of elements stored 
    bit_vector bv; ///<Bitvector where data is stored

  public:
    // @brief Default constructor
    onehotcram() : alphabet_bits(), bv() {}
  
    // @brief Constructor for given data
    onehotcram(const auto& text) : alphabet_bits(), bv() {
      size_t index = 0;
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        for (auto& j: text) {
          bv.insert(index++, (size_t(j) == i));
        }
      }
      size_ = text.size();
    }

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

      //Add 0 or 1 to the correct location for each alphabet symbol
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        bv.insert((i*size_ + index), (i == size_t(elem)));
      }
    }

    void remove(uint32_t index) {
      if (compression == 1) {
        return remove_c1(index);
      }

      size_--;

      //Remove indices for each alphabet element
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        bv.remove(size_*i + index);
      }
    }

    void set(uint32_t index, dtype elem) {

      if (compression == 1) {
        return set_c1(index, elem);
      }

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
      
      for (size_t i = 0; i < ALPHABET_SIZE - 1; i++) {

        uint32_t index_size = i < ALPHABET_SIZE - 1 
          ? alphabet_bits[i + 1] - alphabet_bits[i]
          : bv.size() - alphabet_bits[i];
        
        uint32_t offset = size_ - index_size; 

        if (index >= offset) {
          if (bv.at(alphabet_bits[i] - offset + index)) {
            return static_cast<dtype>(i);
          }
        }
      }

      return static_cast<dtype>(0);
    }
    
    void insert_c1(uint32_t index, dtype elem) {

      uint32_t index_size = alphabet_bits[1] - alphabet_bits[0];

      //Number of virtual leading zeros
      uint32_t offset = size_ - index_size;
      uint32_t total_insertions = 0;
      
      //No extra insertions needed
      if (index > offset) {
        bv.insert((index - offset), size_t(elem) == 0);
        total_insertions++;
      //Add missing zeros until correct index can be accessed
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
      
      //Starting index of the next alphabet entry in bv
      alphabet_bits[1] += total_insertions;
      
      for (size_t i = 1; i < ALPHABET_SIZE - 1; i++) {
      
        //Take previous insertions into account when calculating next size
        index_size = (alphabet_bits[i+1] + total_insertions) - alphabet_bits[i];

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
      index_size = bv.size() - alphabet_bits[last_elem];
      offset = size_ - index_size;

      if (index > offset) {
        bv.insert((alphabet_bits[last_elem] + index - offset),
            size_t(elem) == last_elem);
      } else if (size_t(elem) == last_elem) {
        
        uint32_t zero_insertions = offset - index;

        while (zero_insertions > 0) {
          bv.insert(alphabet_bits[last_elem], 0);
          zero_insertions--;
        }

        bv.insert(alphabet_bits[last_elem], 1);
      } 

      size_++;
    }

    void remove_c1(uint32_t index) {
      
      uint32_t index_size = alphabet_bits[1] - alphabet_bits[0];

      //Number of virtual leading zeros
      uint32_t offset = size_ - index_size;
      uint32_t total_removals = 0;

      if (index >= offset) {
        bv.remove(index - offset);
        index_size--;
        //Remove possible leading zeros
        while (!bv.at(0) && index_size--) {
          bv.remove(0);
          total_removals++;
        }
      } 

      alphabet_bits[1] -= total_removals;

      for (size_t i = 1; i < ALPHABET_SIZE - 1; i++) {

        alphabet_bits[i] -= total_removals;

        index_size = (alphabet_bits[i+1] - total_removals) - alphabet_bits[i];

        offset = size_ - index_size;

        if (index >= offset) {
          bv.remove(alphabet_bits[i] + index - offset);
          index_size--;
          total_removals++;

          while (!bv.at(alphabet_bits[i]) && index_size--) {
            bv.remove(alphabet_bits[i]);
            total_removals++;
          }
        }
        //Remove leading zeros 
        alphabet_bits[i+1] -= total_removals;
      }
       
      uint32_t last_elem = ALPHABET_SIZE - 1;
      index_size = bv.size() - alphabet_bits[last_elem];
      offset = size_ - index_size;

      if (index >= offset) {
        bv.remove(alphabet_bits[last_elem] + index - offset);
        index_size--;

        while (!bv.at(alphabet_bits[last_elem]) && index_size--) {
          bv.remove(alphabet_bits[last_elem]);
        }
      }

      //The number of allowed removals before going over index 
      size_--;
    }

    void set_c1(uint32_t index, dtype elem) {
      
      uint32_t index_size = 0;
      uint32_t offset = 0;

      //Number of insertions/removals
      int32_t total_offset = 0;

      for (size_t i = 0; i < ALPHABET_SIZE; i++) {

        alphabet_bits[i] += total_offset;
        
        //The size of the last index is retrieved from bv.size
        index_size = i < ALPHABET_SIZE - 1
          ? (alphabet_bits[i+1] + total_offset) - alphabet_bits[i]
          : bv.size() - alphabet_bits[i];
        
        //Number of virtual leading zeros
        offset = size_ - index_size;
        
        //Set index to one if correct element
        if (i == size_t(elem)) {
          if (index >= offset) {
            //Adjust to the offset
            bv.set(alphabet_bits[i] + index - offset, 1);
          } else {
            //If index < offset, insert virtual zeros
            uint32_t zero_insertions = offset - index;
            while (zero_insertions > 0) {
              bv.insert(alphabet_bits[i], 0);
              total_offset++;
              zero_insertions--;
            }
            bv.set(alphabet_bits[i], 1);
          }
        //Check if element at index is 1 -> set to 0
        } else if (index >= offset) {
          bv.set(alphabet_bits[i] + index - offset, 0);
          //Remove leading zeros
          while (!bv.at(alphabet_bits[i])) {
            bv.remove(alphabet_bits[i]);
            total_offset--;
          }
        }
      }
    }
};

