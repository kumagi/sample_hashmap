#include <chrono>

#include "hashmap.hpp"

int main(int argc, char** argv) {
  hashmap h;
  int size = std::stoi(std::string(argv[1]));
  for (int i = 0; i < size; ++i) {
    h.insert(i, i*7);
  }
  auto begin = std::chrono::high_resolution_clock::now() ;
  for (int i = 0; i < size; ++i) {
    h.get(i);
  }
  auto end = std::chrono::high_resolution_clock::now() ;

  auto duration = end - begin;
  auto count = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
  std::cout << count << std::endl;
}
