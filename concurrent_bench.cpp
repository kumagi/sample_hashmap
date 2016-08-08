#include <pthread.h>
#include <chrono>
#include <random>

#include "hashmap.hpp"

struct working_set {
  hashmap* hm;
  int idx;
};

void* insert_worker(void* w) {
  working_set& set = *reinterpret_cast<working_set*>(w);
  hashmap& h = *set.hm;
  std::mt19937 rand(set.idx);
  for (int j = 0; j < 100000; ++j) {
    int r = rand();
    if ((r & 7) == 7) {
      h.insert(rand(), r);
    } else {
      h.get(rand());
    }
  }
  return nullptr;
}

int main(int argc, char** argv) {
  int locks = std::stoi(std::string(argv[1]));
  hashmap h(32, locks);

  int nworkers = 4;
  pthread_t workers[nworkers];
  working_set sets[nworkers];

  auto begin = std::chrono::high_resolution_clock::now() ;
  for (int i = 0; i < nworkers; ++i) {
    sets[i].hm = &h;
    sets[i].idx = i;
    pthread_create(&workers[i], nullptr, insert_worker, &sets[i]);
  }
  for (int i = 0; i < nworkers; ++i) {
    pthread_join(workers[i], nullptr);
  }
  auto end = std::chrono::high_resolution_clock::now() ;

  auto duration = end - begin;
  auto count = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
  std::cout << count << " ns" << std::endl;
}
