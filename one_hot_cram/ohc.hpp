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
    onehotcram(const auto& data) : alphabet_bits(), bv() {
      size_ = data.size(); 
      size_t index = 0;
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        bool first_one = false;
        if constexpr (compression == 1) {
          alphabet_bits[i] = index;
          for (auto& j: data) {
            if (!first_one && size_t(j) == i) {
              first_one = true;
            }
            if (first_one) {
              bv.insert(index++, size_t(j) == i);
            }
          }
        } else {
          for (auto& j: data) {
            bv.insert(index++, size_t(j) == i);
          }
        }
      }
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
        
        //Check each index with adjust to the offset
        //No need to check leading zeros
        if (index >= offset) {
          if (bv.at(alphabet_bits[i] - offset + index)) {
            return static_cast<dtype>(i);
          }
        }
      }

      return static_cast<dtype>(0);
    }
    
    void insert_c1(uint32_t index, dtype elem) {

      uint32_t index_size = 0;
      //Number of virtual leading zeros
      uint32_t offset = 0;
      uint32_t total_insertions = 0;
      
      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        
        //Update starting point
        alphabet_bits[i] += total_insertions;

        //Take previous insertions into account when calculating next size
        index_size = i < ALPHABET_SIZE - 1 
          ? (alphabet_bits[i+1] + total_insertions) - alphabet_bits[i]
          : bv.size() - alphabet_bits[i];
        
        //Calculate offset from the left
        offset = size_ - index_size; 
        
        //Regular insertion if index is present
        if (index > offset) {
          bv.insert((alphabet_bits[i] + index - offset), size_t(elem) == i);
          total_insertions++;
        //If not and 1 needs to be inserted, add missing zeros
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
      }
      size_++;
    }

    void remove_c1(uint32_t index) {
      
      uint32_t index_size = 0;
      //Number of virtual leading zeros
      uint32_t offset = 0;
      uint32_t total_removals = 0;

      for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        
        //Adjust to the removals
        alphabet_bits[i] -= total_removals;

        index_size = i < ALPHABET_SIZE - 1
          ? (alphabet_bits[i+1] - total_removals) - alphabet_bits[i]
          : bv.size() - alphabet_bits[i];

        offset = size_ - index_size;
        
        //Removal only needed for the present bits
        if (index >= offset) {
          bv.remove(alphabet_bits[i] + index - offset);
          index_size--;
          total_removals++;
          //Remove leading zeros, if any
          while (!bv.at(alphabet_bits[i]) && index_size--) {
            bv.remove(alphabet_bits[i]);
            total_removals++;
          }
        }
      }
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
          //If the index is empty, one 1 needs to be added
          if (index_size == 0 && index == offset) {
            bv.insert(alphabet_bits[i], 1);
            total_offset++;
            [[unlikely]];
          }
          //If the index is within the present bits
          if (index >= offset) {
            std::cout << alphabet_bits[i] << "\n";
            std::cout << "alphabet_bits[i]: " << alphabet_bits[i] << "\n"
              << "offset: " << offset << "\nindex: " << index << "\ntotal_offset: " << total_offset << "\n";
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
          while (!bv.at(alphabet_bits[i]) && index_size > 0) {
            bv.remove(alphabet_bits[i]);
            total_offset--;
            index_size--;
          }
        }
      }
    }

    void set_c1(auto& data, uint32_t index) {
      for (auto& e: data) {
        set_c1(index++, e); 
      }
    }

    /*
     *@brief Underlying bitvector compression ratio
     *
     * @return Underlying bitvector compression ratio
    */
    double get_bv_compression() const {
      if (size_ == 0) {
        return 0;
      }
      return static_cast<double>(bv.size()) / (ALPHABET_SIZE * size_);
    }

    double get_h0_entropy() {
      return 0;
    }
};

