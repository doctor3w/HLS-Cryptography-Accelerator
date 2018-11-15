#pragma once

template <typename T, int MAX_LEN>
class vector {
 public:
  vector() : size_(0) {}
  vector(int len, T value) : size_(len) {
    for (int i = 0; i < len; i++) {
      data_[i] = value;
    }
  }

  T& operator[](int i) { return data_[i]; }

  const T& operator[](int i) const { return data_[i]; }

  void resize(int new_size, T value = T()) {
    if (new_size < size_) {
      size_ = new_size;
    } else {
      for (int x = size_; x < new_size; x++) {
        data_[x] = value;
      }
      size_ = new_size;
    }
  }

  void assign(int new_size, T value) {
    size_ = new_size;
    for (int x = 0; x < size_; x++) {
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
    for (int x = start; x < size_; x++) {
      data_[x] = data_[end];
      end++;
    }
  }

  void insert(int pos, int count, T value) {
    size_ += count;
    for (int x = size_; x >= pos; x--) {
      data_[x] = data_[x - count];
    }
    for (int x = pos; x < pos + count; x++) {
      data_[x] = value;
    }
  }

 private:
  T data_[MAX_LEN];
  int size_;
};
