#ifndef _AES_H_
#define _AES_H_

#include <hls_stream.h>
#include <stdint.h>
#include "typedefs.h"

#define AES_128 0
#define AES_192 1
#define AES_256 0

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

typedef uint8_t state_t[4][4];

void AES_ECB_encrypt(uint8_t* key, uint8_t* buf); 
void AES_ECB_decrypt(uint8_t* key, uint8_t* buf);

void ecb_encrypt_dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
);

#endif
