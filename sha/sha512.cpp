// http://www.iwar.org.uk/comsec/resources/cipher/sha256-384-512.pdf
//g++ -std=c++11  sha256_code_release/sha256_sse4.o  sha256_code_release/sha256_avx2_rorx2.o sha256_code_release/sha256_avx1.o sha256_code_release/sha256_avx2_rorx8.o sha2.cpp


#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <stdint.h>
#include <inttypes.h>

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

// The first thirty-two bits of the fractional parts of the cube roots of the first sixty-four primes.
uint64_t K[80] = {
  0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
  0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
  0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
  0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
  0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
  0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
  0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
  0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
  0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
  0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
  0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
  0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
  0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
  0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
  0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
  0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
  0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
  0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
  0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
  0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
};


// Rotate right n
inline uint64_t Sn(uint64_t x, int n) { return (x >> n) | (x << (64 - n)); }
inline uint64_t Rn(uint64_t x, int n) { return x >> n; }

inline uint64_t Ch(uint64_t x, uint64_t y, uint64_t z) { return (x & y) ^ (~x & z); }
inline uint64_t Maj(uint64_t x, uint64_t y, uint64_t z) { return (x & y) ^ (x & z) ^ (y & z); }
inline uint64_t CSigma0(uint64_t x) { return Sn(x, 28)^Sn(x, 34)^Sn(x, 39); }
inline uint64_t CSigma1(uint64_t x) { return Sn(x, 14)^Sn(x, 18)^Sn(x, 41); }
inline uint64_t LSigma0(uint64_t x) { return Sn(x, 1)^Sn(x, 8)^Rn(x, 7); }
inline uint64_t LSigma1(uint64_t x) { return Sn(x, 19)^Sn(x, 61)^Rn(x, 6); }


SHA512Hash hashBlock(SHA512Hash inter, const uint64_t msg[16]) {
  uint64_t a = inter.hash[0];
  uint64_t b = inter.hash[1];
  uint64_t c = inter.hash[2];
  uint64_t d = inter.hash[3];
  uint64_t e = inter.hash[4];
  uint64_t f = inter.hash[5];
  uint64_t g = inter.hash[6];
  uint64_t h = inter.hash[7];

  uint64_t W[80];
  for (int i=0; i < 16; i++) {
    W[i] = htobe64(msg[i]);
  }
  for (int i=16; i < 80; i++) {
    W[i] = LSigma1(W[i-2]) + W[i-7] + LSigma0(W[i-15]) + W[i-16];
  }
  // Do 80 rounds
  for (int j=0; j < 80; j++) {
    uint64_t T1 = h + CSigma1(e) + Ch(e, f, g) + K[j] + W[j];
    uint64_t T2 = CSigma0(a) + Maj(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
  }

  SHA512Hash out = inter;
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




SHA512Hash sha512(const void *data, uint64_t nbytes) {
  SHA512Hash curr = SHA512_INIT;
  uint8_t *cdata = (uint8_t*)data;

  uint64_t total = 0;
  while (total <= nbytes) {
    if (nbytes - total >= 128) {
      curr = hashBlock(curr, (uint64_t*)(cdata + total));
    } else {
      uint8_t last[128];
      memset(last, 0, sizeof(last));
      uint64_t remain = nbytes - total;
      memcpy(last, cdata + total, remain);
      last[remain] = 0x80;
      if (128 - (remain + 1) >= 2*sizeof(uint64_t)) { // We have room at the end
        *((uint64_t*)(&last[128-sizeof(uint64_t)])) = htobe64(nbytes*8);
        curr = hashBlock(curr, (uint64_t*)last);
      } else { // Hash as is, then do last block
        curr = hashBlock(curr, (uint64_t*)last);
        memset(last, 0, sizeof(last));
        *((uint64_t*)(&last[128-sizeof(uint64_t)])) = htobe64(nbytes*8);
        curr = hashBlock(curr, (uint64_t*)last);
      }

    }
    total += 128;
  }


  return curr;
}



int main() {
  const char s[] = "hello world";
  char data[1234] = {};
  SHA512Hash test = sha512(data, sizeof(data));

  for (int i=0; i < 8; i++) {
    printf("%" PRIx64 "\n", test.hash[i]);
  }

}
