#include <pthread.h>
#include <vector>
#include <thread>
#include <random>

#include "gtest/gtest.h"
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

TEST(hashmap, concurrent_test) {
  hashmap h;
  int nworkers = 32;
  pthread_t workers[nworkers];
  working_set sets[nworkers];

  for (int i = 0; i < nworkers; ++i) {
    sets[i].hm = &h;
    sets[i].idx = i;
    pthread_create(&workers[i], nullptr, insert_worker, &sets[i]);
  }
  for (int i = 0; i < nworkers; ++i) {
    pthread_join(workers[i], nullptr);
  }
}
