#include "gtest/gtest.h"
#include "hashmap.hpp"

TEST(hashmap, construct) {
  hashmap h;
}

TEST(hashmap, insert) {
  hashmap h;
  h.insert(1,23);
  ASSERT_EQ(h.get(1), 23);
}

TEST(hashmap, remove) {
  hashmap h;
  h.insert(1, 23);
  h.remove(1);
  ASSERT_EQ(0, h.size());
}

TEST(hashmap, insert_multi) {
  hashmap h;
  const int size = 4096;
  for (int i = 0; i < size; ++i) {
    h.insert(i, i * 2);
    ASSERT_EQ(i + 1, h.size());
    for (int j = 0; j < i; ++j) {
      ASSERT_EQ(j * 2, h.get(j));
    }
  }
  for (int i = 0; i < size; ++i) {
    ASSERT_EQ(i * 2, h.get(i));
  }
}

TEST(hashmap, remove_multi) {
  hashmap h;
  const int size = 256;
  for (int i = 0; i < size; ++i) {
    h.insert(i, i * 2);
  }
  for (int i = 0; i < size; ++i) {
    ASSERT_TRUE(h.remove(i));
    for (int j = i + 1; j < size; ++j) {
      ASSERT_EQ(j * 2, h.get(j));
    }
  }
  ASSERT_EQ(0, h.size());
}
