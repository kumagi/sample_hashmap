#include <atomic>
#include <iostream>
#include <vector>
#include <utility>
#include <stdexcept>
#include <memory>
#include <mutex>

#include "MurmurHash2A.cpp"

class hashmap {
  struct kvp {
    kvp(int k, int v) : key(k), value(v), next(nullptr) {}
    int key;
    int value;
    kvp* next;
    ~kvp() {
      if (next) {
        delete next;
      }
    }
  };
  struct table {
    size_t size;
    std::atomic<size_t> entries;
    kvp* array[1];
    table(size_t new_size, size_t ent)
      : size(new_size), entries(ent) {}
    kvp*& operator[](size_t idx) {
      return array[idx];
    }
    kvp* const & operator[](size_t idx) const {
      return array[idx];
    }
    size_t get_entries() const {
      return entries.load();
    }
    void add_entry() {
      entries.fetch_add(1);
    }
  };
 public:
  hashmap(int init_size = 32, int lock_table_size = 16)
    : table_(init_table(init_size, 0)), lock_table_(lock_table_size) {
  }

  void insert(int key, int value) {
    const size_t hash = MurmurHash2A(&key, sizeof(int), 0);
    table& t = *table_.load(std::memory_order_acquire);

    const size_t slot = hash % t.size;
    kvp* const entry = new kvp(key, value);

    {
      const size_t lock_slot = hash % lock_table_.size();
      std::lock_guard<std::mutex> lock(lock_table_[lock_slot]);
      kvp* orig = t[slot];
      t[slot] = entry;
      entry->next = orig;
      t.add_entry();
    }

    // extend if needed
    if (t.get_entries() == t.size) {
      extend(&t);
    }
  }
  int get(int key) const {
    table& t = *table_.load(std::memory_order_acquire);
    const size_t hash = MurmurHash2A(&key, sizeof(int), 0);
    const size_t slot = hash % t.size;

    kvp* target = t[slot];
    if (target != nullptr) {
      // getting slot lock entire iterate list in bucket
      const size_t lock_slot = hash % lock_table_.size();
      std::lock_guard<std::mutex> lock(lock_table_[lock_slot]);

      while (target != nullptr) {
        if (target->key == key) {
          return target->value;
        }
        target = target->next;
      }
    }

    return 0;
    // throw std::runtime_error("not found");
  }

  bool remove(int key) {
    /*
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
  */
    return false;
  }

  size_t size() const {
    table* t = table_.load(std::memory_order_relaxed);
    return t->get_entries();
  }
  /*
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
  */
  ~hashmap() {
    delete_table(table_.load());
  }

 private:
  void extend(table* old) {
    table& old_table = *old;
    std::lock_guard<std::mutex> lock(extend_lock_);
    if (table_.load() != &old_table) { return; }

    std::vector<std::lock_guard<std::mutex>> lock_array;
    for (auto& slot_lock : lock_table_) {
      slot_lock.lock();
    }

    const size_t new_size = old_table.size * 2;
    std::unique_ptr<table> new_table_ptr(init_table(new_size, 0));
    table& new_table = *new_table_ptr;
    for (size_t i = 0; i < old_table.size; ++i) {
      kvp* target = old_table[i];
      while (target != nullptr) {
        const size_t new_hash = MurmurHash2A(&target->key, sizeof(int), 0);
        const size_t slot = new_hash % new_size;
        std::unique_ptr<kvp> entry(new kvp(target->key, target->value));

        kvp* orig = new_table[slot];
        new_table[slot] = entry.get();
        entry->next = orig;
        new_table.add_entry();
        entry.release();

        target = target->next;
      }
    }
    table_.store(new_table_ptr.release());

    for (auto& slot_lock : lock_table_) {
      slot_lock.unlock();
    }
  }

  static table* init_table(size_t size, size_t ent) {
    std::unique_ptr<char[]> ptr(new char[sizeof(size_t) +
                                         sizeof(size_t) +
                                         sizeof(kvp*) * size]);
    std::unique_ptr<table> ret(new(ptr.release()) table(size, ent));
    for (size_t i = 0; i < size; ++i) {
      ret->array[i] = nullptr;
    }
    return ret.release();
  }
  static void delete_table(table* target) {
    std::unique_ptr<char[]> ptr(reinterpret_cast<char*>(target));
    // delete via RAII
  }

  std::atomic<table*> table_;
  mutable std::vector<std::mutex> lock_table_;
  std::mutex extend_lock_;
};
