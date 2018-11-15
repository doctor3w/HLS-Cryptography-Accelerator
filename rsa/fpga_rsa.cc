#include "fpga_rsa.h"
#include "bignum.h"
#include <cassert>
#include <iostream>

typedef Bignum<2 * MAX_BIT_LEN / 32> RsaBignum;
typedef ap_uint<2*MAX_BIT_LEN> BigAp;

ap_uint<MAX_BIT_LEN> fpga_powm(ap_uint<MAX_BIT_LEN> base, ap_uint<MAX_BIT_LEN> exponent, ap_uint<MAX_BIT_LEN> modulus) {
  if (modulus == 1) {
    return 0;
  }

  ap_uint<MAX_BIT_LEN> other = 1;
  RsaBignum result(1);
  RsaBignum b(base);
  RsaBignum e(exponent);
  RsaBignum m(modulus);

  ap_uint<32> ap_zero = 0;
  RsaBignum zero(ap_zero);

  b = b.mod(m);
  while (e > zero) {
    if (e[0] == 1) {
      result = (result * b).mod(m);
    }
    e = e >> 1;
    b = (b*b).mod(m);
  }
  return result.to_ap_uint();
}


