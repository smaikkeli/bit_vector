#include <cstdint>
#include <vector>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <assert.h>
#include "one_hot_cram/ohc.hpp"
#include "bit_vector/bv.hpp"

const int TEST_NUM = 0;
const int SIGMA = 65536;

typedef bv::simple_bv<16, 16384, 64, true, true, true> rle_bv;

void replace_test_ohc(const auto& source, const auto& dest) {

  std::cout << "CRAM OHC BUILD START\n";

  auto start = std::chrono::steady_clock::now();

  onehotcram<uint8_t, rle_bv> ohc(source);

  auto end = std::chrono::steady_clock::now();
  long long elapsed_time_build = std::chrono::duration_cast
    <std::chrono::nanoseconds>(end - start).count();
  elapsed_time_build = static_cast<double>(elapsed_time_build)/1000000000;

  std::cout << "Time taken to build: " << elapsed_time_build << " [s]\n";

  std::cout << "REPLACE TEST\n";
  
  long long elapsed_time_replace = 0;
  for (size_t i = 0; i < dest.size(); i++) {
    start = std::chrono::steady_clock::now();
    ohc.set(i, dest[i]);
    end = std::chrono::steady_clock::now();
    elapsed_time_replace += std::chrono::duration_cast
      <std::chrono::nanoseconds>(end-start).count();
  }
  const double elapsed_time_sec = static_cast<double>(elapsed_time_replace)/1000000000;
  const double elapsed_per_operation = static_cast<double>(elapsed_time_replace) / source.size();
  std::cout << "CRAM REPLACE time elapsed = " << elapsed_time_sec << " [s]\n";
  std::cout << "CRAM REPLACE time per operation = " << elapsed_per_operation << " [ns]\n";
}

void push_csv(int mode,int u,const std::vector<double>& vec,std::ofstream& out){
    out<<mode<<','<<u<<',';
    for(auto val:vec) out<<val<<',';
    out<<'\n';
}

void help() { 
  std::cout << "CRAM TESTING \n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
    help();
    return 0; 
  }

  std::ifstream is_source(argv[1], std::ios::binary);
  is_source.seekg(0, std::ios_base::end);
  size_t size = is_source.tellg();
  is_source.seekg(0, std::ios_base::beg);
  std::vector<uint8_t> source(size/sizeof(uint8_t));
  is_source.read((char*) &source[0], size);
  is_source.close();
  
  std::ifstream is_dest(argv[2], std::ios::binary);
  is_dest.seekg(0, std::ios_base::end);
  size = is_dest.tellg();
  is_dest.seekg(0, std::ios_base::beg);
  std::vector<uint8_t> dest(size/sizeof(uint8_t));
  is_dest.read((char*) &dest[0], size);
  is_dest.close();

  std::ofstream out("test.csv");
  std::cout << "CRAM TEST\n";

  replace_test_ohc(source, dest);
}
