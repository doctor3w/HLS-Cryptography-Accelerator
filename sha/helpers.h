#pragma once

#include <stdint.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))



inline void memcpy_u8(uint8_t *dest, const uint8_t *src, int nbytes) {
  // TODO: unroll this
LOOP:
  for (int i=0; i < nbytes; i++) {
    dest[i] = src[i];
  }
}


inline void memset_u8(uint8_t *dest, uint8_t val, int nbytes) {
LOOP:
  for (int i=0; i < nbytes; i++) {
    dest[i] = val;
  }
}

// Unroll completely
static inline uint64_t read64clear(uint8_t *arr, int sidx, bool clear) {
  uint64_t ret = 0;
  // TODO: unroll this
LOOP:
  for (int i=0; i < sizeof(uint64_t); i++) {
    ret <<= 8;
    ret |= arr[sidx + i];
    if (clear) arr[sidx + i] = 0; // Clear
  }
  return ret;
}
