#pragma once

#include "big_ap_int.h"

template <int WIDTH>
void to_buf(uint32_t* buf, ap_uint<WIDTH> in) {
  for (int x = 0; x < WIDTH / 32; x++) {
    buf[x] = __builtin_bswap32(in(x * 32 + 31, x * 32));
  }
}

template <int WIDTH>
ap_uint<WIDTH> from_buf(uint32_t* buf) {
  ap_uint<WIDTH> result;
  for (int x = 0; x < WIDTH / 32; x++) {
    result(x * 32 + 31, x * 32) = __builtin_bswap32(buf[x]);
  }
  return result;
}
