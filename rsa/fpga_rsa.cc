#include "fpga_rsa.h"
#include "bignum.h"

typedef Bignum<MAX_BIT_LEN / 32> RsaBignum;

ap_uint<MAX_BIT_LEN> fpga_powm(ap_uint<MAX_BIT_LEN> base, ap_uint<MAX_BIT_LEN> exponent, ap_uint<MAX_BIT_LEN> modulus) {
  if (modulus == 1) {
    return 0;
  }
/*
  RsaBignum result(1);
  RsaBignum b(base);
  RsaBignum e(exponent);
  RsaBignum m(modulus);
*/
  ap_uint<MAX_BIT_LEN> result = 1;
  ap_uint<MAX_BIT_LEN> b = base;
  ap_uint<MAX_BIT_LEN> e = exponent;
  ap_uint<MAX_BIT_LEN> m = modulus;

  b = b % m;
  while (e > 0) {
    if (exponent[0] == 1) {
      result = (result * b) % m;
    }
    e = e >> 1;
    b = (b*b) % m;
  }

  return result;

  //return result.to_ap_uint();
}


