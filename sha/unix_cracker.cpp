#include <stdio.h>
#include <assert.h>
//#include <stdlib.h> for crypt()
#include "SHA512.h"
#include "unix_cracker.h"

#ifdef __ORDER_LITTLE_ENDIAN__
#include <endian.h>
SHA512Hash convertEndian(const SHA512Hash in) {
  SHA512Hash ret;
  for (int i=0; i < SHA512Hasher::HASH_SIZE/sizeof(uint64_t); i++) {
    ret.hash64[i] = htobe64(in.hash64[i]);
  }
  return ret;
}

#define FIX_ENDIAN(X) (convertEndian(X))
#else

#define FIX_ENDIAN(X) (X)
#endif

static const uint8_t P[] = {
  42, 21,  0,  1, 43, 22, 23,  2, 44,
  45, 24,  3,  4, 46, 25, 26,  5, 47,
  48, 27,  6,  7, 49, 28, 29,  8, 50,
  51, 30,  9, 10, 52, 31, 32, 11, 53,
  54, 33, 12, 13, 55, 34, 35, 14, 56,
  57, 36, 15, 16, 58, 37, 38, 17, 59,
  60, 39, 18, 19, 61, 40, 41, 20, 62,
  63
};

static const char b64t[65] =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void update_hack(SHA512Hasher &hasher, const uint8_t *buf, uint8_t len) {
  #pragma HLS inline off
  uint8_t stupid_vivado[SHA512Hasher::BLOCK_SIZE];
  memcpy_u8(stupid_vivado, buf, len);
  hasher.update(stupid_vivado, len);
}

static inline SHA512Hash runIters(SHA512Hasher &hasher,
                                  int slen, int pwlen,
                                  const SHA512Hash &A,
                                  const SHA512Hash &DS,
                                  const SHA512Hash &DP,
                                  int nrounds=5000) {
  SHA512Hash C = A;

  for (int i=0; i < nrounds; i++) {
    hasher.reset();

    if (i % 2 == 1) {
      hasher.update(DP.hash8, pwlen);
    } else {
      hasher.update(C.hash8, SHA512Hasher::HASH_SIZE);
    }

    if (i % 3 != 0) {
      hasher.update(DS.hash8, slen);
    }

    if (i % 7 != 0) {
      hasher.update(DP.hash8, pwlen);
    }

    if (i % 2 == 0) {
      hasher.update(DP.hash8, pwlen);
    } else {
      hasher.update(C.hash8, SHA512Hasher::HASH_SIZE);
    }

    C = FIX_ENDIAN(hasher.digest());
  }
  return C;

}

void calc(char hash[86], const uint8_t pwd[MAX_PWD_LEN], const uint8_t pwlen, const uint8_t salt[MAX_SALT_LEN], const uint8_t slen, int nrounds) {
  assert(pwlen <= 64);
  assert(slen <= 64);
  // Compute B
  SHA512Hasher hasher;
  update_hack(hasher, pwd, pwlen);
  update_hack(hasher, salt, slen);
  update_hack(hasher, pwd, pwlen);
  SHA512Hash B = FIX_ENDIAN(hasher.digest());

  hasher.reset();

  // Compute A
  update_hack(hasher, pwd, pwlen);
  update_hack(hasher, salt, slen);
  update_hack(hasher, B.hash8, pwlen);

  uint8_t curr = pwlen;
  for (int i=0; i < 8; i++) {
    if (curr) {
      if (curr & 1) {
        update_hack(hasher, B.hash8, sizeof(B.hash8));
      } else {
        update_hack(hasher, pwd, pwlen);
      }
    }
    curr >>= 1;
  }
  SHA512Hash A = FIX_ENDIAN(hasher.digest());

  hasher.reset();

  // Compute DP
  for (int i=0; i < pwlen; i++) {
    update_hack(hasher, pwd, pwlen);
  }
  SHA512Hash DP = FIX_ENDIAN(hasher.digest());

  hasher.reset();

  // Compute DS
  for (int i=0; i < 16 + A.hash8[0]; i++) {
    update_hack(hasher, salt, slen);
  }
  SHA512Hash DS = FIX_ENDIAN(hasher.digest());


  // Note P is the first N bytes of DP
  // We reuse A for C
  A = runIters(hasher, slen, pwlen, A, DS, DP, nrounds);

  // TODO: unroll this
  for (int i=0; i < 21; i++) {
    uint32_t C = A.hash8[P[3*i]] | (A.hash8[P[3*i + 1]] << 8) | (A.hash8[P[3*i + 2]] << 16);
    for (int j=0; j < 4; j++) {
      hash[4*i + j] = b64t[(C >> (6*j)) & 0x3f];
    }
  }
  // Handle last byte
  uint8_t C = A.hash8[P[63]];
  hash[84] = b64t[C & 0x3f];
  hash[85] = b64t[C >> 6];

}
