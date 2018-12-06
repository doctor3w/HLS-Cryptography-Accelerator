#include <stdio.h>
#include <assert.h>
//#include <stdlib.h> for crypt()
#include "SHA512.h"
#include "unix_cracker.h"

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


static inline SHA512ByteHash runIters(SHA512Hasher &hasher,
                                      uint8_t slen, uint8_t pwlen,
                                      const SHA512ByteHash &A,
                                      const SHA512ByteHash &DS,
                                      const SHA512ByteHash &DP,
                                      int nrounds=5000) {
  SHA512ByteHash C = A;

  // There are 3 cases here: i % 7 | i % 3 | i%2
  // Case 000: SHA512Hasher::HASH_SIZE
  const uint8_t N = SHA512Hasher::HASH_SIZE;
  const int N000 = N + MAX_PWD_LEN;
  const int N001 = MAX_PWD_LEN + N;
  const int N010 = N + MAX_SALT_LEN + MAX_PWD_LEN;
  const int N011 = MAX_PWD_LEN + MAX_SALT_LEN + N;
  const int N100 = MAX_PWD_LEN + MAX_PWD_LEN;
  const int N101 = MAX_PWD_LEN + MAX_PWD_LEN + N;
  const int N110 = N + MAX_SALT_LEN + MAX_PWD_LEN + MAX_PWD_LEN;
  const int N111 = MAX_PWD_LEN + MAX_SALT_LEN +  MAX_PWD_LEN + N;

  uint8_t c000[SHA512_NBYTES(N000)] = {};
  uint8_t c001[SHA512_NBYTES(N001)] = {};
  uint8_t c010[SHA512_NBYTES(N010)] = {};
  uint8_t c011[SHA512_NBYTES(N011)] = {};
  uint8_t c100[SHA512_NBYTES(N100)] = {};
  uint8_t c101[SHA512_NBYTES(N101)] = {};
  uint8_t c110[SHA512_NBYTES(N110)] = {};
  uint8_t c111[SHA512_NBYTES(N111)] = {};


  const uint8_t M000 = N + pwlen;
  const uint8_t M001 = pwlen + N;
  const uint8_t M010 = N + 2*pwlen;
  const uint8_t M011 = pwlen + slen + N;
  const uint8_t M100 = 2*pwlen;
  const uint8_t M101 = 2*pwlen + N;
  const uint8_t M110 = N + slen + pwlen + pwlen;
  const uint8_t M111 = pwlen + slen +  pwlen + N;

  // Setup 000
  memcpy_u8(c000 + N, DP.hash, pwlen);
  c000[M000] = 0x80;
  c000[N000 - 1] = 8*(M000);
  c000[N000 - 2] = 8*(M000) >> 8;

  // Setup 001
  memcpy_u8(c001, DP.hash, pwlen);
  c001[M001] = 0x80;
  c001[N001 - 1] = 8*(M001);
  c001[N001 - 2] = 8*(M001) >> 8;

  // Setup 010
  memcpy_u8(c010 + N, DS.hash, slen);
  memcpy_u8(c010 + N + slen, DP.hash, pwlen);
  c010[M010] = 0x80;
  c010[N010 - 1] = 8*(M010);
  c010[N010 - 2] = 8*(M010) >> 8;

  // Setup 100
  memcpy_u8(c100 + N, DP.hash, pwlen);
  memcpy_u8(c100 + N + pwlen, DP.hash, pwlen);
  c010[M100] = 0x80;
  c010[N100 - 1] = 8*(M100);
  c010[N100 - 2] = 8*(M100) >> 8;

  // Setup 101
  memcpy_u8(c101, DP.hash, pwlen);
  memcpy_u8(c101 + pwlen, DP.hash, pwlen);
  c010[M101] = 0x80;
  c010[N101 - 1] = 8*(M101);
  c010[N101 - 2] = 8*(M101) >> 8;

  // Setup 110
  memcpy_u8(c110 + N, DS.hash, slen);
  memcpy_u8(c110 + N + slen, DP.hash, pwlen);
  memcpy_u8(c110 + N + slen + pwlen, DP.hash, pwlen);
  c010[M110] = 0x80;
  c010[N110 - 1] = 8*(M110);
  c010[N110 - 2] = 8*(M110) >> 8;

  // Setup 111
  memcpy_u8(c111, DP.hash, pwlen);
  memcpy_u8(c111 + pwlen, DS.hash, slen);
  memcpy_u8(c111 + pwlen + slen, DP.hash, pwlen);
  c010[M111] = 0x80;
  c010[N111 - 1] = 8*(M111);
  c010[N111 - 2] = 8*(M111) >> 8;

  for (int i=0; i < nrounds; i++) {
    uint8_t b0 = (i % 2) != 0 ? 1 : 0;
    uint8_t b1 = (i % 3) != 0  ? 1 : 0;
    uint8_t b2 = (i % 7) != 0  ? 1 : 0;
    hasher.reset();

    switch (b2 << 2 | b1 << 1 | b0) {
      case 0b000:
        memcpy_u8(c000, C.hash, N);
        C = SHA512Hasher::hashBlocks(c000, SHA512_NBLOCKS(N000));
        break;

      case 0b001:
        memcpy_u8(c001 + M001 - N, C.hash, N);
        C = SHA512Hasher::hashBlocks(c001, SHA512_NBLOCKS(N001));
        break;


      case 0b010:
        memcpy_u8(c010, C.hash, N);
        C = SHA512Hasher::hashBlocks(c010, SHA512_NBLOCKS(N010));
        break;

      case 0b011:
        memcpy_u8(c011 + M011 - N, C.hash, N);
        C = SHA512Hasher::hashBlocks(c011, SHA512_NBLOCKS(N011));
        break;


      case 0b0100:
        memcpy_u8(c100, C.hash, N);
        C = SHA512Hasher::hashBlocks(c100, SHA512_NBLOCKS(N100));
        break;

      case 0b101:
        memcpy_u8(c101 + M101 - N, C.hash, N);
        C = SHA512Hasher::hashBlocks(c101, SHA512_NBLOCKS(N101));
        break;


      case 0b110:
        memcpy_u8(c110, C.hash, N);
        C = SHA512Hasher::hashBlocks(c110, SHA512_NBLOCKS(N110));
        break;

      case 0b111:
        memcpy_u8(c111 + M111 - N, C.hash, N);
        C = SHA512Hasher::hashBlocks(c111, SHA512_NBLOCKS(N111));
        break;

    }

    // if (i % 2 == 1) {
    //   hasher.update(DP.hash, pwlen);
    // } else {
    //   hasher.update(C.hash, SHA512Hasher::HASH_SIZE);
    // }
    //
    // if (i % 3 != 0) {
    //   hasher.update(DS.hash, slen);
    // }
    //
    // if (i % 7 != 0) {
    //   hasher.update(DP.hash, pwlen);
    // }
    //
    // if (i % 2 == 0) {
    //   hasher.update(DP.hash, pwlen);
    // } else {
    //   hasher.update(C.hash, SHA512Hasher::HASH_SIZE);
    // }
    //
    // C = hasher.byte_digest();
  }
  return C;

}

void calc(char hash[86], const char pwd[MAX_PWD_LEN], const uint8_t pwlen, const char salt[MAX_SALT_LEN], const uint8_t slen, int nrounds) {
  assert(pwlen <= 64);
  assert(slen <= 64);
  // Compute B
  SHA512Hasher hasher;
  hasher.update(pwd, pwlen);
  hasher.update(salt, slen);
  hasher.update(pwd, pwlen);
  SHA512ByteHash B = hasher.byte_digest();

  hasher.reset();

  // Compute A
  hasher.update(pwd, pwlen);
  hasher.update(salt, slen);
  hasher.update(B.hash, pwlen);

  uint8_t curr = pwlen;
  for (int i=0; i < 8; i++) {
    if (curr) {
      if (curr & 1) {
        hasher.update(B.hash, sizeof(B.hash));
      } else {
        hasher.update(pwd, pwlen);
      }
    }
    curr >>= 1;
  }
  SHA512ByteHash A = hasher.byte_digest();

  hasher.reset();

  // Compute DP
  for (int i=0; i < pwlen; i++) {
    hasher.update(pwd, pwlen);
  }
  SHA512ByteHash DP = hasher.byte_digest();

  hasher.reset();

  // Compute DS
  for (int i=0; i < 16 + A.hash[0]; i++) {
    hasher.update(salt, slen);
  }
  SHA512ByteHash DS = hasher.byte_digest();


  // Note P is the first N bytes of DP
  // We reuse A for C
  A = runIters(hasher, slen, pwlen, A, DS, DP, nrounds);

  // TODO: unroll this
  for (int i=0; i < 21; i++) {
    uint32_t C = A.hash[P[3*i]] | (A.hash[P[3*i + 1]] << 8) | (A.hash[P[3*i + 2]] << 16);
    for (int j=0; j < 4; j++) {
      hash[4*i + j] = b64t[(C >> (6*j)) & 0x3f];
    }
  }
  // Handle last byte
  uint8_t C = A.hash[P[63]];
  hash[84] = b64t[C & 0x3f];
  hash[85] = b64t[C >> 6];

}
