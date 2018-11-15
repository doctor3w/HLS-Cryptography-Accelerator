#include "fpga_rsa.h"

ap_uint<MAX_BIT_LEN> fpga_powm(ap_uint<MAX_BIT_LEN> base, ap_uint<MAX_BIT_LEN> exponent, ap_uint<MAX_BIT_LEN> modulus) {
  if (modulus == 1) {
    return 0;
  }
  ap_uint<MAX_BIT_LEN> result = 1;
  base = base % modulus;
  while (exponent > 0) {
    if (exponent[0] == 1) {
      result = (result * base) % modulus;
    }
    exponent = exponent >> 1;
    base = (base*base) % modulus;
  }

  return result;
}


