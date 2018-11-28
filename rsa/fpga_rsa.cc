#include "fpga_rsa.h"
#include <cassert>
#include <iostream>
#include "bignum.h"

const int BITS_PER_DIGIT = 128;
typedef Bignum<2 * MAX_BIT_LEN / BITS_PER_DIGIT, BITS_PER_DIGIT> RsaBignum;
typedef ap_uint<2 * MAX_BIT_LEN> BigAp;

const int SUPER_DUPER_FOO=MAX_BIT_LEN;

ap_uint<MAX_BIT_LEN> fpga_powm(ap_uint<MAX_BIT_LEN> base,
                               ap_uint<MAX_BIT_LEN> exponent,
                               ap_uint<MAX_BIT_LEN> modulus) {
  if (modulus == 1) {
    return 0;
  }

  ap_uint<MAX_BIT_LEN> other = 1;
  RsaBignum result(1);
  RsaBignum b(base);
  RsaBignum e(exponent);
  RsaBignum m(modulus);
  RsaBignum zero(0);
  assert(b.to_ap_uint() == base);
  assert(e.to_ap_uint() == exponent);
  assert(m.to_ap_uint() == modulus);
  assert(zero.to_ap_uint() == 0);
  assert(result.to_ap_uint() == other);

  b = b % m;
  base = base % modulus;
  assert(b.to_ap_uint() == base);
  POWM_LOOP: for (int i = 0; i < MAX_BIT_LEN; i++) {
    if (e > zero) {
      if (e[0] == 1) {
        result = (result * b) % m;
        other = (other * base) % modulus;
        assert(result.to_ap_uint() == other);
      }
      e = e >> 1;
      exponent = exponent >> 1;
      assert(e.to_ap_uint() == exponent);
      b = (b * b);
      assert(b.to_ap_uint() == base * base);
      b = b % m;
      base = (base * base) % modulus;
      assert(b.to_ap_uint() == base);
    }
  }
  return result.to_ap_uint();
}
