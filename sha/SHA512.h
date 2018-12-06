
#pragma once

#include <stdint.h>

#include <string.h>
#include "helpers.h"

#define SHA512_NBLOCKS(N) (((N) + 1 + 2*sizeof(uint64_t))/(SHA512Hasher::BLOCK_SIZE))

#define SHA512_NBYTES(N) (SHA512_NBLOCKS(N) * SHA512Hasher::BLOCK_SIZE)


struct SHA512Hash {
  uint64_t hash[8];

  bool operator==(const SHA512Hash& rhs)
  {
      return !memcmp(hash, rhs.hash, sizeof(uint64_t[8]));
  }
};

struct SHA512ByteHash {
  uint8_t hash[64];
};


class SHA512Hasher {
public:
  SHA512Hasher();
  void reset();
  // len <= 128
  SHA512Hash digest();
  static SHA512ByteHash hashBlocks(uint8_t *msg, uint8_t nblocks);
  SHA512ByteHash byte_digest();
  void update(const void *msgp, uint8_t len);

  static const uint8_t HASH_SIZE = 64;
  static const uint8_t BLOCK_SIZE = 128;
private:
  SHA512Hash state;
  uint8_t buf[BLOCK_SIZE]; // TODO: This should be partitioned in chunks of 8
  uint8_t bsize;
  uint64_t total;

  static SHA512Hash hashBlock(SHA512Hash start, uint8_t *msg, bool clear=true);
  // Rotate right n
  static inline uint64_t Sn(uint64_t x, int n) { return (x >> n) | (x << (64 - n)); }
  static inline uint64_t Rn(uint64_t x, int n) { return x >> n; }

  static inline uint64_t Ch(uint64_t x, uint64_t y, uint64_t z) { return (x & y) ^ (~x & z); }
  static inline uint64_t Maj(uint64_t x, uint64_t y, uint64_t z) { return (x & y) ^ (x & z) ^ (y & z); }
  static inline uint64_t CSigma0(uint64_t x) { return Sn(x, 28)^Sn(x, 34)^Sn(x, 39); }
  static inline uint64_t CSigma1(uint64_t x) { return Sn(x, 14)^Sn(x, 18)^Sn(x, 41); }
  static inline uint64_t LSigma0(uint64_t x) { return Sn(x, 1)^Sn(x, 8)^Rn(x, 7); }
  static inline uint64_t LSigma1(uint64_t x) { return Sn(x, 19)^Sn(x, 61)^Rn(x, 6); }

};
