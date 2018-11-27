#ifndef __FPGA_SHA_512_H
#define __FPGA_SHA_512_H

#include <stdint.h>
#include <ap_int.h>

#include "config.h"
#include "typedefs.h"

// Inspired by OSSSwapConstInt from XNU 
#define portable_htobe16(x) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
#define portable_htobe32(x) ((portable_htobe16(x) << 16) | (portable_htobe16((x) >> 16)))
#define portable_htobe64(x) ((((uint64_t)portable_htobe32(x)) << 32) | (portable_htobe32((x) >> 32)))

struct SHA512Hash {
  uint64_t hash[8];
};

const SHA512Hash SHA512_INIT = {
  {
    0x6a09e667f3bcc908,
    0xbb67ae8584caa73b,
    0x3c6ef372fe94f82b,
    0xa54ff53a5f1d36f1,
    0x510e527fade682d1,
    0x9b05688c2b3e6c1f,
    0x1f83d9abfb41bd6b,
    0x5be0cd19137e2179
  }
};

SHA512Hash hashBlock(SHA512Hash inter, const uint64_t msg[16]);

#endif
