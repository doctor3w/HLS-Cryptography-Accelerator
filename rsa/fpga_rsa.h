#include "rsa_config.h"
#include <ap_int.h>


template <int BIT_LEN>
ap_uint<BIT_LEN> fpga_powm(ap_uint<BIT_LEN> base, ap_uint<BIT_LEN> exponent, ap_uint<BIT_LEN> modulus) {
  if (modulus == 1) {
    return 0;
  }
  ap_uint<BIT_LEN> result = 1;
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

template <int BIT_LEN>
ap_uint<BIT_LEN> fpga_rsa_block(ap_uint<BIT_LEN> data, ap_uint<BIT_LEN> n, ap_uint<BIT_LEN> e) {
  return fpga_powm(data, e, n);
}

