#pragma once

// Adapter from:
// https://github.com/possibly-wrong/precision/blob/master/math_Unsigned.h

#include <stdint.h>
#include "big_ap_int.h"
#include "vector.h"

template <int MAX_DIGITS>
class Bignum {
 public:
  typedef uint32_t Digit;
  typedef uint64_t Wigit;
  static const unsigned BITS = 32;

  //  Bignum(Digit u = 0) : digits(1, u) {}

  Bignum(ap_uint<MAX_DIGITS* BITS> value = 0) {
  INIT_LOOP:
    for (int i = 0; i < MAX_DIGITS; i++) {
      digits.push_back((value >> i * BITS) & 0xFFFFFFFF);
    }
    trim();
  }

  ap_uint<MAX_DIGITS * BITS> to_ap_uint() {
    ap_uint<MAX_DIGITS* BITS> result = 0;
CONVERT_DIGITS: for (int i = 0; i < MAX_DIGITS; i++) {
      if (i >= digits.size()) break;
CONVERT_BITS: for (unsigned int b = 0; b < BITS; b++) {
        result[i * BITS + b] = (digits[i] >> b) & 1;
      }
    }
    return result;
  }

  int operator[](int index) {
    return (digits[index / BITS] >> (index % BITS)) & 1;
  }

  friend Bignum operator+(Bignum u, const Bignum& v) {
    u += v;
    return u;
  }

  Bignum& operator+=(const Bignum& rhs) {
    const int n = rhs.digits.size();
    if (digits.size() < n) {
      digits.resize(n, 0);
    }
    int j = 0;
    Wigit k = 0;
    for (; j < MAX_DIGITS; ++j) {
      if (j >= n) break;
      k = k + digits[j] + rhs.digits[j];
      digits[j] = static_cast<Digit>(k);
      k >>= BITS;
    }
    for (; k != 0 && j < MAX_DIGITS; ++j) {
      if (j >= digits.size()) break;
      k += digits[j];
      digits[j] = static_cast<Digit>(k);
      k >>= BITS;
    }
    if (k != 0) {
      digits.push_back(1);
    }
    return *this;
  }

  friend Bignum operator-(Bignum u, const Bignum& v) {
    u -= v;
    return u;
  }

  Bignum& operator-=(const Bignum& rhs) {
    int j = 0;
    Wigit k = 0;
    for (; j < MAX_DIGITS; ++j) {
      if (j >= rhs.digits.size()) break;
      k = k + digits[j] - rhs.digits[j];
      digits[j] = static_cast<Digit>(k);
      k = ((k >> BITS) ? -1 : 0);
    }
    for (; k != 0 && j < MAX_DIGITS; ++j) {
      if (j >= digits.size()) break;
      k += digits[j];
      digits[j] = static_cast<Digit>(k);
      k = ((k >> BITS) ? -1 : 0);
    }
    trim();
    return *this;
  }

  friend Bignum operator*(const Bignum& u, const Bignum& v) {
    const int m = u.digits.size();
    const int n = v.digits.size();
    Bignum w;
    w.digits.resize(m + n, 0);
  FOOBAR:
    for (int j = 0; j < MAX_DIGITS; ++j) {
      if (j >= n) break;
      Wigit k = 0;
    EGGPLANT:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        if (i >= m) break;
        k += static_cast<Wigit>(u.digits[i]) * v.digits[j] + w.digits[i + j];
        w.digits[i + j] = static_cast<Digit>(k);
        k >>= BITS;
      }
      w.digits[j + m] = static_cast<Digit>(k);
    }
    w.trim();
    return w;
  }

  Bignum& operator*=(const Bignum& rhs) {
    *this = (*this) * rhs;
    return *this;
  }

  friend Bignum operator/(const Bignum& u, const Bignum& v) {
    Bignum q, r;
    u.divide(v, q, r);
    return q;
  }

  Bignum& operator/=(const Bignum& rhs) {
    Bignum r;
    divide(rhs, *this, r);
    return *this;
  }

  friend Bignum operator%(const Bignum& u, const Bignum& v) {
    Bignum q, r;
    u.divide(v, q, r);
    return r;
  }

  Bignum& operator%=(const Bignum& rhs) {
    Bignum q;
    divide(rhs, q, *this);
    return *this;
  }

  void divide(Bignum v, Bignum& q, Bignum& r) const {
    r.digits = digits;
    const int n = v.digits.size();
    if (digits.size() < n) {
      q.digits.assign(1, 0);
      return;
    }

    // Normalize divisor (v[n-1] >= BASE/2).
    unsigned d = BITS;
    Digit vn = v.digits.back();
  NORMALIZE:
    for (int magic = 0; magic < MAX_DIGITS; magic++) {
     if (vn == 0) break;
     vn >>= 1;
     --d;
    }
    v <<= d;
    r <<= d;
    vn = v.digits.back();

    // Ensure first single-digit quotient (u[m-1] < v[n-1]).
    r.digits.push_back(0);
    const int m = r.digits.size();
    q.digits.resize(m - n);
    Bignum w;
    w.digits.resize(n + 1);
    const Wigit MAX_DIGIT = (static_cast<Wigit>(1) << BITS) - 1;
    int j = m - n;
  DIVIDE:
    for (int x = 0; x < MAX_DIGITS; x++) {
      if (j == 0) break;
      j--;
      // Estimate quotient digit.
      Wigit a =
          (static_cast<Wigit>(r.digits[j + n]) << BITS | r.digits[j + n - 1]) /
          vn;
      Wigit qhat = a < MAX_DIGIT ? a : MAX_DIGIT;

      // Compute partial product (w = qhat * v).
      Wigit k = 0;
    PARTIAL:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        if (i >= n) break;
        k += qhat * v.digits[i];
        w.digits[i] = static_cast<Digit>(k);
        k >>= BITS;
      }
      w.digits[n] = static_cast<Digit>(k);

      // Check if qhat is too large (u - w < 0).
      bool is_trial = true;
    SEARCH:
      for (int foobar = 0; foobar < 3; foobar++) {
        if (!is_trial) {
          break;
        }
        int i = n;
      COMPARE:
        for (int y = 0; y < MAX_DIGITS; y++) {
          if (i == 0 || r.digits[j + i] != w.digits[i]) {
            break;
          }
          i--;
        }
        if ((is_trial = (r.digits[j + i] < w.digits[i]))) {
          // Adjust partial product (w -= v).
          --qhat;
          k = 0;
        FOOBAR:
          for (int i = 0; i < MAX_DIGITS; ++i) {
            if (i >= n) break;
            k = k + w.digits[i] - v.digits[i];
            w.digits[i] = static_cast<Digit>(k);
            k = ((k >> BITS) ? -1 : 0);
          }
          w.digits[n] = static_cast<Digit>(k + w.digits[n]);
        }
      }
      q.digits[j] = static_cast<Digit>(qhat);

      // Compute partial remainder (u -= w).
      k = 0;
    REM:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        if (i >= n) break;
        k = k + r.digits[j + i] - w.digits[i];
        r.digits[j + i] = static_cast<Digit>(k);
        k = ((k >> BITS) ? -1 : 0);
      }
    }

    // Denormalize remainder.
    q.trim();
    r.digits.resize(n);
    r >>= d;
  }

  friend Bignum operator<<(Bignum u, int v) {
    u <<= v;
    return u;
  }

  Bignum& operator<<=(int rhs) {
    if (digits.back() != 0 && rhs != 0) {
      const int n = rhs / BITS;
      digits.insert(0, n, 0);
      rhs -= n * BITS;
      Wigit k = 0;
    HLS_IS_SO_BROKEN:
      for (int j = 0; j < MAX_DIGITS; ++j) {
        if (j + n >= digits.size()) break;
        k |= static_cast<Wigit>(digits[j + n]) << rhs;
        digits[j + n] = static_cast<Digit>(k);
        k >>= BITS;
      }
      if (k != 0) {
        digits.push_back(static_cast<Digit>(k));
      }
    }
    return *this;
  }

  friend Bignum operator>>(Bignum u, int v) {
    u >>= v;
    return u;
  }

  Bignum& operator>>=(int rhs) {
    const int n = rhs / BITS;
    if (n >= digits.size()) {
      digits.assign(1, 0);
    } else {
      digits.erase(0, n);
      rhs -= n * BITS;
      Wigit k = 0;
      int j = digits.size();
      for (int x = 0; x < MAX_DIGITS; x++) {
        if (j == 0) break;
        j--;
        k = k << BITS | digits[j];
        digits[j] = static_cast<Digit>(k >> rhs);
        k = static_cast<Digit>(k);
      }
      trim();
    }
    return *this;
  }

  friend Bignum operator^(Bignum u, const Bignum& v) {
    u ^= v;
    return u;
  }

  Bignum& operator^=(const Bignum& rhs) {
    const int n = rhs.digits.size();
    if (digits.size() < n) {
      digits.resize(n, 0);
    }
    for (int j = 0; j < MAX_DIGITS; ++j) {
      if (j >= n) break;
      digits[j] ^= rhs.digits[j];
    }
    trim();
    return *this;
  }

  friend bool operator<(const Bignum& u, const Bignum& v) {
    const int m = u.digits.size();
    int n = v.digits.size();
    if (m != n) {
      return (m < n);
    }
COMPARE: for (int x = 0; x < MAX_DIGITS; x++) {
      n--;
      if (n == 0 || u.digits[n] != v.digits[n]) break;
    }
    return (u.digits[n] < v.digits[n]);
  }

  friend bool operator>(const Bignum& u, const Bignum& v) { return (v < u); }

  friend bool operator<=(const Bignum& u, const Bignum& v) { return !(v < u); }

  friend bool operator>=(const Bignum& u, const Bignum& v) { return !(u < v); }

  friend bool operator==(const Bignum& u, const Bignum& v) {
    return (u.digits == v.digits);
  }

  friend bool operator!=(const Bignum& u, const Bignum& v) {
    return (u.digits != v.digits);
  }

  // Return 1 + floor(log2(u)), or 0 for u == 0.
  int bits() const {
    int count = (digits.size() - 1) * BITS;
    for (Digit u = digits.back(); u != 0; u >>= 1, ++count)
      ;
    return static_cast<int>(count);
  }

  Digit to_uint() const { return digits[0]; }

 private:
  vector<Digit, 1 + MAX_DIGITS> digits;

  void trim() {
  TRIM:
    for (int x = 0; x < MAX_DIGITS; x++) {
      if (digits.back() == 0 && digits.size() > 1) {
        digits.pop_back();
      } else {
        break;
      }
    }
  }
};
