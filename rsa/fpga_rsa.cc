#include "fpga_rsa.h"
#include <cassert>
#include <iostream>
#include "pragmas.h"

void dut(hls::stream<bit32_t>& strm_in, hls::stream<bit32_t>& strm_out) {
  RsaNum base = read_rsa_num(strm_in);
  RsaNum exponent = read_rsa_num(strm_in);
  RsaNum modulus = read_rsa_num(strm_in);
  write_rsa_num(fpga_powm(base, exponent, modulus), strm_out);
}

RsaNum read_rsa_num(hls::stream<bit32_t>& in) {
  HLS_PRAGMA(inline);
  RsaNum result = 0;
READ_LOOP:
  for (int x = 0; x < MAX_BIT_LEN / 32; x++) {
    HLS_PRAGMA(unroll);
    result(x * 32 + 31, x * 32) = in.read();
  }
  return result;
}

void write_rsa_num(RsaNum num, hls::stream<bit32_t>& out) {
  HLS_PRAGMA(inline);
WRITE_LOOP:
  for (int x = 0; x < MAX_BIT_LEN / 32; x++) {
    HLS_PRAGMA(unroll);
    out.write(num(x * 32 + 31, x * 32));
  }
}

RsaNum fpga_powm(RsaNum base, RsaNum exponent, RsaNum modulus) {
  HLS_PRAGMA(inline);
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
