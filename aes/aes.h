#ifndef _AES_H_
#define _AES_H_

#include <hls_stream.h>
#include <stdint.h>
#include "typedefs.h"

#define AES_128 1
#define AES_192 0
#define AES_256 0

#ifndef NUM_BLOCKS
#define NUM_BLOCKS 31
#endif

#define BLOCKLEN 16 // 16 bytes = 128 bits
#define Nb 4

#if defined(AES_256) && (AES_256 == 1)
  #define KEYLEN 32
  #define Nk 8
  #define Nr 14
  #define KEYLEN_EXP 240
#elif defined(AES_192) && (AES_192 == 1)
  #define KEYLEN 24
  #define Nk 6
  #define Nr 12
  #define KEYLEN_EXP 208
#else
  #define KEYLEN 16
  #define Nk 4
  #define Nr 10
  #define KEYLEN_EXP 176
#endif

typedef ap_uint<KEYLEN * 8> aes_key_t;
typedef ap_uint<128> roundkey_t[KEYLEN_EXP * 8 / 128];

void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
);

#endif
