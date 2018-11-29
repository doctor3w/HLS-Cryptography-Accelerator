#include "fpga_rsa.h"
#include <cassert>
#include <iostream>
#include "bignum.h"

const int BITS_PER_DIGIT = 32;
typedef Bignum<2 * MAX_BIT_LEN / BITS_PER_DIGIT, BITS_PER_DIGIT> RsaBignum;
typedef ap_uint<2 * MAX_BIT_LEN> BigAp;
// typedef BigAp RsaBignum;

const int SUPER_DUPER_FOO = MAX_BIT_LEN;

ap_uint<MAX_BIT_LEN> fpga_powm(ap_uint<MAX_BIT_LEN> base,
                               ap_uint<MAX_BIT_LEN> exponent,
                               ap_uint<MAX_BIT_LEN> modulus) {
  if (modulus == 1) {
    return 0;
  }

  RsaBignum result(1);
  RsaBignum b(base);
  RsaBignum e(exponent);
  RsaBignum m(modulus);
  RsaBignum zero(0);

  b = b % m;
POWM_LOOP:
  for (int i = 0; i < MAX_BIT_LEN; i++) {
    if (e > zero) {
      if (e[0] == 1) {
        result = (result * b) % m;
      }
      e = e >> 1;
      b = (b * b) % m;
    }
  }
  return result.to_ap_uint();
}
