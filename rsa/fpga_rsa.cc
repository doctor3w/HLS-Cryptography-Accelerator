#include "fpga_rsa.h"
#include <cassert>
#include <iostream>


void fpga_powm_stream(hls::stream<uint32_t>& in, hls::stream<uint32_t>& out) {
   RsaNum base = read_rsa_num(in);
   RsaNum exponent = read_rsa_num(in);
   RsaNum modulus = read_rsa_num(in);
   write_rsa_num(fpga_powm(base, exponent, modulus), out);
}

RsaNum read_rsa_num(hls::stream<uint32_t>& in) {
  HLS_PRAGMA(inline);
  RsaNum result = 0;
  for (int x = 0; x < MAX_BIT_LEN / 32; x++) {
    result(x * 32 + 31, x * 32) = in.read();
  }
  return result;
}

void write_rsa_num(RsaNum num, hls::stream<uint32_t>& out) {
  HLS_PRAGMA(inline);
  for (int x = 0; x < MAX_BIT_LEN / 32; x++) {
    out.write(num(x * 32 + 31, x * 32));
  }
}

RsaNum fpga_powm(RsaNum base,
                               RsaNum exponent,
                               RsaNum modulus) {
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
