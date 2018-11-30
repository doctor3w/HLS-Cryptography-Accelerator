#pragma once

#include "big_ap_int.h"
#include "fpga_rsa.h"

RsaBignum from_buf(uint32_t* buf) {
  RsaBignum result;
  int i = 0;
  for (int x = 0; x < MAX_BIT_LEN / BITS_PER_DIGIT; x++) {
    ap_uint<INT32S_PER_DIGIT * 32> temp = 0;
    for (int y = 0; y < INT32S_PER_DIGIT; y++) {
      temp(32 * y + 31, 32 * y) = buf[i];
      i++;
    }
    result.set_block(x, temp);
  }
  return result;
}

void to_buf(uint32_t* buf, RsaBignum in) {
  int i = 0;
  for (int x = 0; x < MAX_BIT_LEN / BITS_PER_DIGIT; x++) {
    for (int y = 0; y < INT32S_PER_DIGIT; y++) {
      buf[i] = in.block(x)(32 * y + 31, 32 * y);
      i++;
    }
  }
}

