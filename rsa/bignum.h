#pragma once

// Adapter from:
// https://github.com/possibly-wrong/precision/blob/master/math_Unsigned.h

#include <stdint.h>
#include "big_ap_int.h"
#include "pragmas.h"

template <int MAX_DIGITS, int BITS>
class Bignum {
 public:
  typedef ap_uint<MAX_DIGITS * BITS> BigAp;
  typedef ap_uint<BITS> Digit;
  typedef ap_uint<2 * BITS> Wigit;
  struct Internal {
    Internal() {
      HLS_PRAGMA(resource variable=data core=RAM_1P_BRAM)
    }
    Digit data[MAX_DIGITS];
  };

  Bignum(BigAp value = 0) {
    HLS_PRAGMA(inline off);
    for (int x = 0; x < MAX_DIGITS; x++) {
      set_block(x, value(x * BITS + BITS - 1, x * BITS));
    }
  }

  BigAp to_ap_uint() {
    HLS_PRAGMA(inline off);
    BigAp result = 0;
    for (int x = 0; x < MAX_DIGITS; x++) {
      result(x * BITS + BITS - 1, x * BITS) = block(x);
    }
    return result;
  }

  int operator[](int index) const {
    HLS_PRAGMA(inline off);
    return block(index / BITS)[index % BITS];
  }

  void set_block(int i, Digit v) {
    HLS_PRAGMA(inline off);
    if (i < MAX_DIGITS) {
      digits.data[i] = v;
    }
  }

  Digit block(int i) const {
    HLS_PRAGMA(inline off);
    if (i < MAX_DIGITS) {
      return digits.data[i];
    } else {
      return 0;
    }
  }

  int size() const {
    HLS_PRAGMA(inline off);
    for (int result = MAX_DIGITS - 1; result >= 0; result--) {
      if (block(result) != 0) {
        return result + 1;
      }
    }
    return 1;
  }

  friend Bignum operator*(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    const int m = u.size();
    const int n = v.size();
    Bignum w;
  OUTER:
    for (int j = 0; j < MAX_DIGITS; ++j) {
      if (j >= n) break;
      Wigit k = 0;
    INNER:
      for (int i = 0; i < MAX_DIGITS; ++i) {
        if (i >= m) break;
        k += static_cast<Wigit>(u.block(i)) * v.block(j) + w.block(i + j);
        w.set_block(i + j, static_cast<Digit>(k));
        k >>= BITS;
      }
      w.set_block(j + m, static_cast<Digit>(k));
    }
    return w;
  }

  Bignum& operator*=(const Bignum& rhs) {
    HLS_PRAGMA(inline off);
    *this = (*this) * rhs;
    return *this;
  }

  friend Bignum operator/(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    Bignum q, r;
    u.divide(v, q, r);
    return q;
  }

  Bignum& operator/=(const Bignum& rhs) {
    HLS_PRAGMA(inline off);
    Bignum r;
    divide(rhs, *this, r);
    return *this;
  }

  friend Bignum operator%(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    Bignum q, r;
    u.divide(v, q, r);
    return r;
  }

  Bignum& operator%=(const Bignum& rhs) {
    HLS_PRAGMA(inline off);
    Bignum q;
    divide(rhs, q, *this);
    return *this;
  }

  void divide(Bignum v, Bignum& q, Bignum& r) const {
    HLS_PRAGMA(inline off);
    r.digits = digits;
    const int n = v.size();
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
    v <<= d;
    r <<= d;
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
      bool is_trial = true;
    SEARCH:
      for (int foobar = 0; foobar < 3; foobar++) {
        if (!is_trial) {
          break;
        }
        int i = n;
      COMPARE:
        for (int y = 0; y < MAX_DIGITS; y++) {
          if (i == 0 || r.block(j + i) != w.block(i)) {
            break;
          }
          i--;
        }
        if ((is_trial = (r.block(j + i) < w.block(i)))) {
          // Adjust partial product (w -= v).
          --qhat;
          k = 0;
        ADJUST:
          for (int i = 0; i < MAX_DIGITS; ++i) {
            if (i >= n) break;
            k = k + w.block(i) - v.block(i);
            w.set_block(i, static_cast<Digit>(k));
            k = ((k >> BITS) ? -1 : 0);
          }
          w.set_block(n, static_cast<Digit>(k + w.block(n)));
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

    for (int x = 0; x < MAX_DIGITS; x++) {
      if (x >= n) {
        r.set_block(x, 0);
      }
    }
    // Denormalize remainder.
    r >>= d;
  }

  friend Bignum operator<<(Bignum u, int v) {
    HLS_PRAGMA(inline off);
    u <<= v;
    return u;
  }

  Bignum& operator<<=(int rhs) {
    HLS_PRAGMA(inline off);
    if (block(size() - 1) != 0 && rhs != 0) {                                                               
      const int n = rhs / BITS;                                                                         
EXPAND: for (int x = MAX_DIGITS - 1; x >= 0; x--) {
        if (x < n) break;
        set_block(x, block(x - n));
      }
      rhs -= n * BITS;                                                                                  
      Wigit k = 0;                                                                                      
    SHIFT:                                                                                              
      for (int j = 0; j < MAX_DIGITS; ++j) {                                                            
        if (j + n >= size()) break;                                                              
        k |= static_cast<Wigit>(block(j + n)) << rhs;                                                  
        set_block(j + n, static_cast<Digit>(k));
        k >>= BITS;                                                                                     
      }                                                                                                 
      if (k != 0) {                                                                                     
        set_block(size(), (static_cast<Digit>(k)));                                                        
      }                                                                                                 
    }                                                                                                   
    return *this;          
  }

  friend Bignum operator>>(Bignum u, int v) {
    HLS_PRAGMA(inline off);
    u >>= v;
    return u;
  }

  Bignum& operator>>=(int rhs) {
    HLS_PRAGMA(inline off);
    const int n = rhs / BITS;                                                                           
    if (n >= size()) {
CLEAR: for (int i = 0; i < MAX_DIGITS; i++) {
        set_block(i, 0);
      }                                                                           
    } else {
      int s = size();                                                                                           
SHRINK: for (int x = 0; x <= MAX_DIGITS; x++) {
        if (x >= s - n) {
          set_block(x, 0);
        } else {
          set_block(x, block(x + n));
        }
      }
      rhs -= n * BITS;                                                                                  
      Wigit k = 0;                                                                                      
      int j = s + n;
SHIFT: for (int x = 0; x < MAX_DIGITS; x++) {                                                            
        if (j == 0) break;                                                                              
        j--;                                                                                            
        k = k << BITS | block(j);
        set_block(j, static_cast<Digit>(k >> rhs));
        k = static_cast<Digit>(k);                                                                      
      }                                                                                                 
    }                                                                                                   
    return *this;  
  }

  friend bool operator<(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
 const int m = u.size();                                                       
    int n = v.size();                                                                            
    if (m != n) {                                                                                       
      return (m < n);                                                                                   
    }                                                                                                   
COMPARE: for (int x = 0; x < MAX_DIGITS; x++) {                                                         
      n--;                                                                                              
      if (n == 0 || u.block(n) != v.block(n)) break;                                                  
    }                                                                                                   
    return (u.block(n) < v.block(n));      
  }

  friend bool operator>(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    return (v < u);
  }

  friend bool operator<=(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    return !(v < u);
  }

  friend bool operator>=(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    return !(u < v);
  }

  friend bool operator==(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    return (u.digits == v.digits);
  }

  friend bool operator!=(const Bignum& u, const Bignum& v) {
    HLS_PRAGMA(inline off);
    return (u.digits != v.digits);
  }

 private:
  Internal digits;
};
