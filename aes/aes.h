#ifndef _AES_
#define _AES_

#include <stdint.h>

#define AES_128 1
#define AES_192 0
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

void aes_ebc_encrypt(uint8_t* buf, uint8_t* key);
void aes_ebc_decrypt(uint8_t* buf, uint8_t* key);

#endif
