#pragma once

// Adapted from (very heavily modified):
// https://github.com/possibly-wrong/precision/blob/master/math_Unsigned.h

#include <stdint.h>
#include "big_ap_int.h"
#include "pragmas.h"

template <typename T, int LEN>
struct Array {
  friend bool operator==(const Array<T, LEN>& u, const Array<T, LEN>& v) {
    HLS_PRAGMA(inline);
    for (int x = 0; x < LEN; x++) {
      if (u.data[x] != v.data[x]) {
        return false;
      }
    }
    return true;
  }

  friend bool operator!=(const Array<T, LEN>& u, const Array<T, LEN>& v) {
    HLS_PRAGMA(inline);
    return !(u == v);
  }
  T data[LEN];
};

template <int MAX_DIGITS, int BITS>
class Bignum {
 public:
  typedef ap_uint<BITS> Digit;
  typedef ap_uint<2 * BITS> Wigit;

  Bignum(Digit low = 0) {
    HLS_PRAGMA(inline);
    set_block(0, low);
  INIT:
    for (int x = 1; x < MAX_DIGITS; x++) {
      HLS_PRAGMA(unroll);
      set_block(x, 0);
    }
  }

  int operator[](int index) const {
    HLS_PRAGMA(inline);
    return block(index / BITS)[index % BITS];
  }

  void set(int index, int value) {
    HLS_PRAGMA(inline);
    if (index < MAX_DIGITS * BITS) {
      digits.data[index / BITS][index % BITS] = value;
    }
  }

  void set_block(int i, Digit v) {
    HLS_PRAGMA(inline);
    if (i < MAX_DIGITS) {
      digits.data[i] = v;
    }
  }

  Digit block(int i) const {
    HLS_PRAGMA(inline);
    if (i < MAX_DIGITS) {
      return digits.data[i];
    } else {
      return 0;
    }
  }

  int size() const {
    HLS_PRAGMA(inline);
  SIZE:
    for (int result = MAX_DIGITS - 1; result >= 0; result--) {
      HLS_PRAGMA(unroll);
      if (block(result) != 0) {
        return result + 1;
      }
    }
    return 1;
  }

  friend Bignum operator*(const Bignum& u, const Bignum& v) {
    Bignum w;
  OUTER:
    for (int j = 0; j < MAX_DIGITS; ++j) {
      Wigit k = 0;
    INNER:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        k += static_cast<Wigit>(u.block(i)) * v.block(j) + w.block(i + j);
        w.set_block(i + j, static_cast<Digit>(k));
        k >>= BITS;
      }
    }
    return w;
  }

  friend Bignum operator%(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline);
    Bignum q, r;
    u.divide(v, q, r);
    return r;
  }

  void divide(Bignum v, Bignum& q, Bignum& r) const {
    r.digits = digits;
    const int n = v.size();
  ZERO:
    for (int i = 0; i < MAX_DIGITS; i++) {
      q.set_block(i, 0);
    }
    if (size() < n) {
      return;
    }

    // Normalize divisor (v[n-1] >= BASE/2).
    unsigned d = BITS;
    Digit vn = v.block(n - 1);
  NORMALIZE:
    for (int magic = 0; magic < BITS; magic++) {
      if (vn == 0) break;
      vn >>= 1;
      --d;
    }
    v.lshift_safe(d);
    r.lshift_safe(d);
    vn = v.block(n - 1);

    // Ensure first single-digit quotient (u[m-1] < v[n-1]).
    const int m = r.size() + 1;
    Bignum w;
    const Wigit MAX_DIGIT = (static_cast<Wigit>(1) << BITS) - 1;
    int j = m - n;
  DIVIDE:
    for (int x = 0; x < MAX_DIGITS; x++) {
      if (j == 0) break;
      j--;
      // Estimate quotient digit.
      Wigit a = (static_cast<Wigit>(r.block(j + n)) << BITS |
                 static_cast<Wigit>(r.block(j + n - 1))) /
                vn;
      Wigit qhat = a < MAX_DIGIT ? a : MAX_DIGIT;

      // Compute partial product (w = qhat * v).
      Wigit k = 0;
    PARTIAL:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        if (i >= n) break;
        k += qhat * v.block(i);
        w.set_block(i, static_cast<Digit>(k));
        k >>= BITS;
      }
      w.set_block(n, static_cast<Digit>(k));

      // Check if qhat is too large (u - w < 0).
    SEARCH:
      for (int trial = 0; trial < 3; trial++) {
        int i = n;
      COMPARE:
        for (int y = 0; y < MAX_DIGITS; y++) {
          if (i == 0 || r.block(j + i) != w.block(i)) {
            break;
          }
          i--;
        }
        if (r.block(j + i) < w.block(i)) {
          // Adjust partial product (w -= v).
          --qhat;
          k = 0;
        ADJUST:
          for (int i = 0; i < MAX_DIGITS; ++i) {
            HLS_PRAGMA(pipeline);
            if (i >= n) break;
            k = k + w.block(i) - v.block(i);
            w.set_block(i, static_cast<Digit>(k));
            k = ((k >> BITS) ? -1 : 0);
          }
          w.set_block(n, static_cast<Digit>(k + w.block(n)));
        } else {
          break;
        }
      }
      q.set_block(j, static_cast<Digit>(qhat));

      // Compute partial remainder (u -= w).
      k = 0;
    REM:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        if (i >= n) break;
        k = k + r.block(j + i) - w.block(i);
        r.set_block(j + i, static_cast<Digit>(k));
        k = ((k >> BITS) ? -1 : 0);
      }
    }

  CLEAR_UPPER:
    for (int x = 0; x < MAX_DIGITS; x++) {
      if (x >= n) {
        r.set_block(x, 0);
      }
    }
    // Denormalize remainder.
    r.rshift_safe(d);
  }

  void lshift_safe(int rhs) {
    HLS_PRAGMA(inline);
    int s = size();
    if (block(s - 1) != 0 && rhs != 0) {
      Wigit k = 0;
    SHIFT:
      for (int j = 0; j < MAX_DIGITS; ++j) {
        if (j >= s) break;
        k |= static_cast<Wigit>(block(j)) << rhs;
        set_block(j, static_cast<Digit>(k));
        k >>= BITS;
      }
      if (k != 0) {
        set_block(s, (static_cast<Digit>(k)));
      }
    }
  }

  void rshift_safe(int rhs) {
    HLS_PRAGMA(inline);
    Wigit k = 0;
    int j = size();
  SHIFT:
    for (int x = 0; x < MAX_DIGITS; x++) {
      if (j == 0) break;
      j--;
      k = k << BITS | block(j);
      set_block(j, static_cast<Digit>(k >> rhs));
      k = static_cast<Digit>(k);
    }
  }

  friend bool operator<(const Bignum& u, const Bignum& v) {
    const int m = u.size();
    int n = v.size();
    if (m != n) {
      return (m < n);
    }
  COMPARE:
    for (int x = 0; x < MAX_DIGITS; x++) {
      HLS_PRAGMA(unroll);
      n--;
      if (n == 0 || u.block(n) != v.block(n)) break;
    }
    return (u.block(n) < v.block(n));
  }

  friend bool operator>(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline);
    return (v < u);
  }

  friend bool operator<=(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline);
    return !(v < u);
  }

  friend bool operator>=(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline);
    return !(u < v);
  }

  friend bool operator==(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline);
    return (u.digits == v.digits);
  }

  friend bool operator!=(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline);
    return (u.digits != v.digits);
  }

 private:
  Array<Digit, MAX_DIGITS> digits;
};
