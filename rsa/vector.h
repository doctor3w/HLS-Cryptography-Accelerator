#pragma once

template <typename T, int MAX_LEN>
class vector {
 public:
  vector() : size_(0) {}
  vector(int len, T value) : size_(len) {
INIT: for (int i = 0; i < MAX_LEN; i++) {
      if (i >= len) break;
      data_[i] = value;
    }
  }

  T& operator[](int i) { return data_[i]; }

  const T& operator[](int i) const { return data_[i]; }

  void resize(int new_size, T value = T()) {
    if (new_size < size_) {
      size_ = new_size;
    } else {
RESIZE_LOOP: for (int x = 0; x < MAX_LEN; x++) {
        if (x + size_ >= new_size) break;
        data_[x + size_] = value;
      }
      size_ = new_size;
    }
  }

  void assign(int new_size, T value) {
    size_ = new_size;
ASSIGN: for (int x = 0; x < MAX_LEN; x++) {
      if (x >= size_) break;
      data_[x] = value;
    }
  }

  void push_back(T x) {
    data_[size_] = x;
    size_++;
  }

  void pop_back() { size_--; }

  T& back() { return data_[size_ - 1]; }

  T& front() { return data_[0]; }

  int size() const { return size_; }

  void erase(int start, int end) {
    size_ -= end - start;
ERASE: for (int x = start; x < MAX_LEN; x++) {
      if (x >= size_) break;
      data_[x] = data_[end];
      end++;
    }
  }

  void insert(int pos, int count, T value) {
    size_ += count;
SHIFT: for (int x = MAX_LEN; x >= 0; x--) {
      if (x < pos) break;
      if (x <= size_) {
        data_[x] = data_[x - count];
      }
    }
COPY: for (int x = 0 ; x < MAX_LEN; x++) {
      if (x >= count) break;
      data_[x + pos] = value;
    }
  }

 private:
  T data_[MAX_LEN];
  int size_;
};
