// http://www.iwar.org.uk/comsec/resources/cipher/sha256-384-512.pdf
//g++ -std=c++11  sha256_code_release/sha256_sse4.o  sha256_code_release/sha256_avx2_rorx2.o sha256_code_release/sha256_avx1.o sha256_code_release/sha256_avx2_rorx8.o sha2.cpp 


#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <assert.h>
#include <string.h>
#include <endian.h>
#include <inttypes.h>

#include "include.h"

struct SHA256Hash {
  uint32_t hash[8];
};


const SHA256Hash SHA256_INIT = {
  {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
  }
};

uint8_t *padd(const void *buf, const size_t len, size_t *buflen) {
  size_t resid = (len + 1) % 64;
  size_t k = (resid <= 56) ? 56 - resid : (64 - resid) + 56;
  *buflen = len + 1 + k + sizeof(uint64_t);
  assert(!(*buflen % 64));
  uint8_t *res = (uint8_t*)calloc(*buflen, 1);
  memcpy(res, buf, len);
  res[len] = 0x80;
  *((uint64_t *)(&res[*buflen - sizeof(uint64_t)])) = htobe64(len*8);
  return res;
}


template <void(*F)(void*, uint32_t*, uint64_t)>
SHA256Hash sha256(const void *data, size_t len) {
  size_t buflen;
  uint8_t *padded = padd(data, len, &buflen);
  SHA256Hash out = SHA256_INIT;
  F(padded, out.hash, buflen / 64);
  free(padded);
  return out;
}


int main() {
  const char s[] = "hello world";

  SHA256Hash out = sha256<sha256_sse4>(&s, strlen(s));
  SHA256Hash out2 = sha256<sha256_avx>(&s, strlen(s));
  SHA256Hash out3 = sha256<sha256_rorx>(&s, strlen(s));
  SHA256Hash out4 = sha256<sha256_rorx_x8ms>(&s, strlen(s));

  uint32_t *buf = (uint32_t*)out.hash;
  for (int i=0; i < 8; i++) {
    printf("%08x\n", buf[i]);
  }


}
