#include <iostream>
#include <vector>
#include <utility>
#include <stdexcept>

#include "MurmurHash2A.cpp"

class hashmap {
 public:
  hashmap() : table_(32), entries_(0) {
  }

  void insert(int key, int value) {
    const size_t hash = MurmurHash2A(&key, sizeof(int), 0);
    const size_t slot = hash % table_.size();
    kvp* const entry = new kvp(key, value);
    if (table_[slot] == nullptr) {
      table_[hash % table_.size()] = entry;
    } else {
      kvp* target = table_[slot];
      while (target->next != nullptr) {
        target = target->next;
      }
      target->next = entry;
    }
    ++entries_;

    // extend if needed
    if (entries_ != table_.size()) {
      return;
    }

    const size_t new_size = table_.size() * 2;
    std::vector<kvp*> new_table(new_size);
    for (size_t i = 0; i < table_.size(); ++i) {
      kvp* target = table_[i];
      while (target != nullptr) {
        const size_t new_hash = MurmurHash2A(&target->key, sizeof(int), 0);
        const size_t new_slot = new_hash % new_size;
        kvp* const entry = new kvp(target->key, target->value);

        if (new_table[new_slot] == nullptr) {
          new_table[new_slot] = entry.release();
        } else {
          kvp* ptr = new_table[new_slot];
          while (ptr->next != nullptr) {
            ptr = ptr->next;
          }
          ptr->next = entry;
        }
        target = target->next;
      }
    }
    table_.swap(new_table);
  }

  int get(int key) const {
    const size_t hash = MurmurHash2A(&key, sizeof(int), 0);
    const size_t slot = hash % table_.size();

    kvp* target = table_[slot];
    while (target != nullptr) {
      if (target->key == key) {
        return target->value;
      }
      target = target->next;
    }
    throw std::runtime_error("not found");
  }

  bool remove(int key) {
    const size_t hash = MurmurHash2A(&key, sizeof(int), 0);
    const size_t slot = hash % table_.size();

    kvp** prev = &table_[slot];
    while (*prev != nullptr) {
      kvp* curr = (*prev);
      if (curr->key == key) {
        --entries_;
        (*prev)->next = curr->next;
        delete curr;
        return true;
      }
      prev = &curr->next;
    }
    return false;
  }

  size_t size() const {
    return entries_;
  }

  void dump() const {
    for (size_t i = 0; i < table_.size(); ++i) {
      std::cout << "[" << i << "] ";

      const kvp* target = table_[i];
      int ctr = 0;
      while (target != nullptr) {
        if (0 < ctr) {
          std::cout << " --> ";
        }
        if (table_[i] == nullptr) {
          std::cout << "(null)";
        } else {
          std::cout << target->key << " => " << target->value;
        }
        target = target->next;
        ++ctr;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

 private:
  struct kvp {
    kvp(int k, int v) : key(k), value(v), next(nullptr) {}
    int key;
    int value;
    kvp* next;
  };

  std::vector<kvp*> table_;
  size_t entries_;
};
