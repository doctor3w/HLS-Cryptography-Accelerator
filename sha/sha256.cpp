// http://www.iwar.org.uk/comsec/resources/cipher/sha256-384-512.pdf
//g++ -std=c++11  sha256_code_release/sha256_sse4.o  sha256_code_release/sha256_avx2_rorx2.o sha256_code_release/sha256_avx1.o sha256_code_release/sha256_avx2_rorx8.o sha2.cpp


#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <stdint.h>


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

// The first thirty-two bits of the fractional parts of the cube roots of the first sixty-four primes.
uint32_t K[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};


// Rotate right n
inline uint32_t Sn(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
inline uint32_t Rn(uint32_t x, int n) { return x >> n; }

inline uint32_t Ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
inline uint32_t Maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
inline uint32_t CSigma0(uint32_t x) { return Sn(x, 2)^Sn(x, 13)^Sn(x, 22); }
inline uint32_t CSigma1(uint32_t x) { return Sn(x, 6)^Sn(x, 11)^Sn(x, 25); }
inline uint32_t LSigma0(uint32_t x) { return Sn(x, 7)^Sn(x, 18)^Rn(x, 3); }
inline uint32_t LSigma1(uint32_t x) { return Sn(x, 17)^Sn(x, 19)^Rn(x, 10); }


SHA256Hash hashBlock(SHA256Hash inter, const uint32_t msg[16]) {
  uint32_t a = inter.hash[0];
  uint32_t b = inter.hash[1];
  uint32_t c = inter.hash[2];
  uint32_t d = inter.hash[3];
  uint32_t e = inter.hash[4];
  uint32_t f = inter.hash[5];
  uint32_t g = inter.hash[6];
  uint32_t h = inter.hash[7];

  uint32_t W[64];
  for (int i=0; i < 16; i++) {
    W[i] = htobe32(msg[i]);
  }
  for (int i=16; i < 64; i++) {
    W[i] = LSigma1(W[i-2]) + W[i-7] + LSigma0(W[i-15]) + W[i-16];
  }
  // Do 64 rounds
  for (int j=0; j < 64; j++) {
    uint32_t T1 = h + CSigma1(e) + Ch(e, f, g) + K[j] + W[j];
    uint32_t T2 = CSigma0(a) + Maj(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
  }

  SHA256Hash out = inter;
  out.hash[0] += a;
  out.hash[1] += b;
  out.hash[2] += c;
  out.hash[3] += d;
  out.hash[4] += e;
  out.hash[5] += f;
  out.hash[6] += g;
  out.hash[7] += h;

  return out;
}




SHA256Hash sha256(const void *data, uint64_t nbytes) {
  SHA256Hash curr = SHA256_INIT;
  uint8_t *cdata = (uint8_t*)data;

  uint64_t total = 0;
  while (total <= nbytes) {
    if (nbytes - total >= 64) {
      curr = hashBlock(curr, (uint32_t*)(cdata + total));
    } else {
      uint8_t last[64];
      memset(last, 0, sizeof(last));
      uint64_t remain = nbytes - total;
      memcpy(last, cdata + total, remain);
      last[remain] = 0x80;
      if (64 - (remain + 1) >= sizeof(uint64_t)) { // We have room at the end
        *((uint64_t*)(&last[64-sizeof(uint64_t)])) = htobe64(nbytes*8);
        curr = hashBlock(curr, (uint32_t*)last);
      } else { // Hash as is, then do last block
        curr = hashBlock(curr, (uint32_t*)last);
        memset(last, 0, sizeof(last));
        *((uint64_t*)(&last[64-sizeof(uint64_t)])) = htobe64(nbytes*8);
        curr = hashBlock(curr, (uint32_t*)last);
      }

    }
    total += 64;
  }


  return curr;
}



int main() {
  const char s[] = "hello world";
  char data[127] = {};
  SHA256Hash test = sha256(data, sizeof(data));

  for (int i=0; i < 8; i++) {
    printf("%08x\n", test.hash[i]);
  }

}
