#pragma once
#include "pragmas.h"
#include <cassert>

template <typename T, int MAX_LEN>
class vector {
 public:
  vector() : size_(0) {
    HLS_PRAGMA(inline);
//    HLS_PRAGMA(array_partition variable=data_ complete);
  }
  vector(int len, T value) : size_(len) {
  HLS_PRAGMA(inline);
  INIT:
    for (int i = 0; i < MAX_LEN; i++) {
    HLS_PRAGMA(unroll);
      if (i >= len) break;
      data_[i] = value;
    }
  }

  T& operator[](int i) {
    HLS_PRAGMA(inline);
    return data_[i];
  }

  const T& operator[](int i) const {
    HLS_PRAGMA(inline);
          return data_[i];
  }

  void resize(int new_size, T value = T()) {
    HLS_PRAGMA(inline);
    if (new_size < size_) {
      size_ = new_size;
    } else {
    RESIZE_LOOP:
      for (int x = 0; x < MAX_LEN; x++) {
    HLS_PRAGMA(unroll);
        if (x + size_ >= new_size) break;
        data_[x + size_] = value;
      }
      size_ = new_size;
    }
  }

  void assign(int new_size, T value) {
    HLS_PRAGMA(inline);
    size_ = new_size;
  ASSIGN:
    for (int x = 0; x < MAX_LEN; x++) {
    HLS_PRAGMA(unroll);
      if (x >= size_) break;
      data_[x] = value;
    }
  }

  void push_back(T x) {
    HLS_PRAGMA(inline);
    data_[size_] = x;
    size_++;
  }

  void pop_back() {
    HLS_PRAGMA(inline);
          size_--; }

  T& back() {
    HLS_PRAGMA(inline);
          return data_[size_ - 1]; }

  T& front() {
    HLS_PRAGMA(inline);
          return data_[0]; }

  int size() const {
    HLS_PRAGMA(inline);
          return size_; }

  void erase(int start, int end) {
    HLS_PRAGMA(inline);
    size_ -= end - start;
  ERASE:
    for (int x = start; x < MAX_LEN; x++) {
    HLS_PRAGMA(unroll);
      if (x >= size_) break;
      data_[x] = data_[end];
      end++;
    }
  }

  void insert(int pos, int count, T value) {
    HLS_PRAGMA(inline);
    size_ += count;
  SHIFT:
    for (int x = MAX_LEN - 1; x >= 0; x--) {
    HLS_PRAGMA(unroll);
      if (x < pos) break;
      if (x <= size_) {
        data_[x] = data_[x - count];
      }
    }
  COPY:
    for (int x = 0; x < MAX_LEN; x++) {
    HLS_PRAGMA(unroll);
      if (x >= count) break;
      data_[x + pos] = value;
    }
  }

 private:
  T data_[MAX_LEN];
  int size_;
};
