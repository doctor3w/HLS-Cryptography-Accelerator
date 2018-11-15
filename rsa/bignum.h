#pragma once

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
    for (int i = 0; i < MAX_DIGITS; i++) {
      digits.push_back((value >> i * BITS) & 0xFFFFFFFF);
    }
    trim();
  }

  ap_uint<MAX_DIGITS * BITS> to_ap_uint() {
    ap_uint<MAX_DIGITS* BITS> result = 0;
    for (int i = 0; i < digits.size(); i++) {
      for (int b = 0; b < BITS; b++) {
        result[i * BITS + b] = (digits[i] >> b) & 1;
      }
    }
    return result;
  }

  int operator[](int index) {
    return (digits[index / BITS] >> (index % BITS)) & 1;
  }

  Bignum operator++(int) {
    Bignum w(*this);
    ++(*this);
    return w;
  }

  Bignum& operator++() {
    for (int j = 0; j < digits.size() && ++digits[j] == 0; ++j)
      ;
    if (digits.back() == 0) {
      digits.push_back(1);
    }
    return *this;
  }

  Bignum operator--(int) {
    Bignum w(*this);
    --(*this);
    return w;
  }

  Bignum& operator--() {
    for (int j = 0; j < digits.size() && digits[j]-- == 0; ++j)
      ;
    trim();
    return *this;
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
    for (; j < n; ++j) {
      k = k + digits[j] + rhs.digits[j];
      digits[j] = static_cast<Digit>(k);
      k >>= BITS;
    }
    for (; k != 0 && j < digits.size(); ++j) {
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
    for (; j < rhs.digits.size(); ++j) {
      k = k + digits[j] - rhs.digits[j];
      digits[j] = static_cast<Digit>(k);
      k = ((k >> BITS) ? -1 : 0);
    }
    for (; k != 0 && j < digits.size(); ++j) {
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
    for (int j = 0; j < n; ++j) {
      Wigit k = 0;
      for (int i = 0; i < m; ++i) {
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

  /*
   * ComMsgMgrException: Command "gen_rtl fpga_powm_operator% -style xilinx
   * -tracefl mytrace -tracefmt vcd -traceopt all -f -lang sc -o
   * /home/jng55/SuperAwesomeFastCryptoHLSAcceleratorThing/rsa/rsa.prj/solution1/syn/systemc/fpga_modpowm_operator
   * -synmodules fpga_powm_trim fpga_powm_insert fpga_powm_resize
   * fpga_powm_divide fpga_powm_operator% fpga_powm_operator* fpga_powm_mod
   * fpga_powm_operator>> fpga_powm_operator*.1 fpga_powm " : Invalid c_string
   * in format.
   *
   * ComMsgMgrException: Command "gen_rtl fpga_powm_operator% -style xilinx -f
   * -lang vhdl -o
   * /home/jng55/SuperAwesomeFastCryptoHLSAcceleratorThing/rsa/rsa.prj/solution1/syn/vhdl/fpga_modpowm_operator
   * " : Invalid c_string in format.
   *
   * ComMsgMgrException: Command "gen_rtl fpga_powm_operator% -style xilinx -f
   * -lang vlog -o
   * /home/jng55/SuperAwesomeFastCryptoHLSAcceleratorThing/rsa/rsa.prj/solution1/syn/verilog/fpga_modpowm_operator
   * " : Invalid c_string in format.
   */

  /*
  friend Bignum operator% (const Bignum& u, const Bignum& v)
  {
      Bignum q, r;
      u.divide(v, q, r);
      return r;
  }
  */

  Bignum mod(const Bignum& v) {
    Bignum q, r;
    (*this).divide(v, q, r);
    return r;
  }

  /*
  Bignum& operator%= (const Bignum& rhs)
  {
      Bignum q;
      divide(rhs, q, *this);
      return *this;
  }
  */

  void divide(Bignum v, Bignum& q, Bignum& r) const {
    r.digits = digits;
    const int n = v.digits.size();
    if (digits.size() < n) {
      q.digits.assign(1, 0);
      return;
    }

    // Normalize divisor (v[n-1] >= BASE/2).
    unsigned d = BITS;
    for (Digit vn = v.digits.back(); vn != 0; vn >>= 1, --d)
      ;
    v <<= d;
    r <<= d;
    const Digit vn = v.digits.back();

    // Ensure first single-digit quotient (u[m-1] < v[n-1]).
    r.digits.push_back(0);
    const int m = r.digits.size();
    q.digits.resize(m - n);
    Bignum w;
    w.digits.resize(n + 1);
    const Wigit MAX_DIGIT = (static_cast<Wigit>(1) << BITS) - 1;
    for (int j = m - n; j-- != 0;) {
      // Estimate quotient digit.
      Wigit a =
          (static_cast<Wigit>(r.digits[j + n]) << BITS | r.digits[j + n - 1]) /
          vn;
      Wigit qhat = a < MAX_DIGIT ? a : MAX_DIGIT;

      // Compute partial product (w = qhat * v).
      Wigit k = 0;
      for (int i = 0; i < n; ++i) {
        k += qhat * v.digits[i];
        w.digits[i] = static_cast<Digit>(k);
        k >>= BITS;
      }
      w.digits[n] = static_cast<Digit>(k);

      // Check if qhat is too large (u - w < 0).
      bool is_trial = true;
      while (is_trial) {
        int i = n;
        for (; i != 0 && r.digits[j + i] == w.digits[i]; --i)
          ;
        if ((is_trial = (r.digits[j + i] < w.digits[i]))) {
          // Adjust partial product (w -= v).
          --qhat;
          k = 0;
          for (int i = 0; i < n; ++i) {
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
      for (int i = 0; i < n; ++i) {
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
      for (int j = n; j < digits.size(); ++j) {
        k |= static_cast<Wigit>(digits[j]) << rhs;
        digits[j] = static_cast<Digit>(k);
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
      for (int j = digits.size(); j-- != 0;) {
        k = k << BITS | digits[j];
        digits[j] = static_cast<Digit>(k >> rhs);
        k = static_cast<Digit>(k);
      }
      trim();
    }
    return *this;
  }

  friend Bignum operator&(Bignum u, const Bignum& v) {
    u &= v;
    return u;
  }

  Bignum& operator&=(const Bignum& rhs) {
    const int n = rhs.digits.size();
    if (digits.size() > n) {
      digits.resize(n);
    }
    for (int j = 0; j < digits.size(); ++j) {
      digits[j] &= rhs.digits[j];
    }
    trim();
    return *this;
  }

  Bignum and_not(const Bignum& v) const {
    Bignum u(*this);
    const int n = v.digits.size();
    if (u.digits.size() > n) {
      u.digits.resize(n);
    }
    for (int j = 0; j < u.digits.size(); ++j) {
      u.digits[j] &= ~v.digits[j];
    }
    u.trim();
    return u;
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
    for (int j = 0; j < n; ++j) {
      digits[j] ^= rhs.digits[j];
    }
    trim();
    return *this;
  }

  friend Bignum operator|(Bignum u, const Bignum& v) {
    u |= v;
    return u;
  }

  Bignum& operator|=(const Bignum& rhs) {
    const int n = rhs.digits.size();
    if (digits.size() < n) {
      digits.resize(n, 0);
    }
    for (int j = 0; j < n; ++j) {
      digits[j] |= rhs.digits[j];
    }
    return *this;
  }

  friend bool operator<(const Bignum& u, const Bignum& v) {
    const int m = u.digits.size();
    int n = v.digits.size();
    if (m != n) {
      return (m < n);
    }
    for (--n; n != 0 && u.digits[n] == v.digits[n]; --n)
      ;
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
    while (digits.back() == 0 && digits.size() > 1) {
      digits.pop_back();
    }
  }
};
