#include "fpga_rsa.h"
#include <cassert>
#include <iostream>
#include "pragmas.h"

void dut(hls::stream<bit32_t>& strm_in, hls::stream<bit32_t>& strm_out) {
  RsaBignum base = read_rsa_num(strm_in);
  RsaBignum exponent = read_rsa_num(strm_in);
  RsaBignum modulus = read_rsa_num(strm_in);
  write_rsa_num(fpga_powm(base, exponent, modulus), strm_out);
}

RsaBignum read_rsa_num(hls::stream<bit32_t>& in) {
  HLS_PRAGMA(inline);
  RsaBignum result;
READ_LOOP:
  for (int x = 0; x < MAX_BIT_LEN / BITS_PER_DIGIT; x++) {
    ap_uint<INT32S_PER_DIGIT * 32> temp;
  DIGIT_LOOP:
    for (int y = 0; y < INT32S_PER_DIGIT; y++) {
      HLS_PRAGMA(unroll);
      temp(32 * y + 31, 32 * y) = in.read();
    }
    result.set_block(x, temp);
  }
  return result;
}

void write_rsa_num(RsaBignum num, hls::stream<bit32_t>& out) {
  HLS_PRAGMA(inline);
WRITE_LOOP:
  for (int x = 0; x < MAX_BIT_LEN / BITS_PER_DIGIT; x++) {
  DIGIT_LOOP:
    for (int y = 0; y < INT32S_PER_DIGIT; y++) {
      out.write(num.block(x)(32 * y + 31, 32 * y));
    }
  }
}

RsaBignum fpga_powm(RsaBignum base, RsaBignum exponent, RsaBignum modulus) {
  HLS_PRAGMA(inline);
  RsaBignum result(1);

  if (modulus == result) {
    return RsaBignum(0);
  }

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
  return result;
}
