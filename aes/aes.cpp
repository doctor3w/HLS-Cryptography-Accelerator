#include "aes.h"

typedef bit128_t state_t;

static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t rcon[11] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

#define sBoxVal(num) (sbox[(num)])
#define sBoxInv(num) (rsbox[(num)])

static void keyExpansion(roundkey_t roundKey, aes_key_t key) {
//  #pragma HLS inline
  unsigned i, j, k;
  bit32_t tempa, tempb;
  bit8_t temp;

  // copy in key
#if AES_256 == 1
  roundKey[0] = key.range(127, 0);
  roundKey[1] = key.range(255, 128);
#elif AES_192 == 1
  roundKey[0] = key.range(127, 0);
  roundKey[1](63, 0) = key.range(191, 128);
#else
  roundKey[0] = key;
#endif

EXPAND_LOOP:  
  for (i = Nk; i < Nb * (Nr + 1); ++i) {
    #pragma HLS unroll
    j = (i - 1) >> 2; // array index
    k = (i - 1) % 4;
    tempa = roundKey[j].range(k * 32 + 31, k * 32);

    if (i % Nk == 0) {
      // RotWord
      tempa.rrotate(8);

      // SubWord
      tempa(7, 0)   = sBoxVal(tempa.range(7, 0));
      tempa(15, 8)  = sBoxVal(tempa.range(15, 8));
      tempa(23, 16) = sBoxVal(tempa.range(23, 16));
      tempa(31, 24) = sBoxVal(tempa.range(31, 24));
  
      // Rcon
      tempa(7, 0) = tempa.range(7, 0) ^ rcon[i/Nk];
    }
#if defined(AES_256) && (AES_256 == 1)
    if (i % Nk == 4) {
      tempa(7, 0)   = sBoxVal(tempa.range(7, 0));
      tempa(15, 8)  = sBoxVal(tempa.range(15, 8));
      tempa(23, 16) = sBoxVal(tempa.range(23, 16));
      tempa(31, 24) = sBoxVal(tempa.range(31, 24)); 
    }
#endif
    j = (i - Nk) >> 2;
    k = ((i - Nk) % 4) * 32;
    tempb = roundKey[j].range(k + 31, k);
    j = i >> 2;
    k = (i % 4) * 32;
    roundKey[j].range(k + 31, k) = tempa ^ tempb;
  }
}

static state_t AddRoundKey(uint8_t round, state_t state, roundkey_t roundKey) {
//  #pragma HLS inline
  return state ^ roundKey[round];
}

static state_t SubBytes(state_t state) {
//  #pragma HLS inline 
  uint8_t i, j, k;
SUBBYTE_1:  
  for (i = 0; i < 4; ++i) {
    #pragma HLS unroll
  SUBBYTE_2:    
    for (j = 0; j < 4; ++j) {
      #pragma HLS unroll
      k = i * 32 + j * 8;
      state(k + 7, k) = sBoxVal(state.range(k + 7, k));
    }
  }
  return state;
}

static state_t ShiftRows(state_t state) {
//  #pragma HLS inline
  bit8_t temp;
  // no rotate of row 0
  
  // rotate row 1
  temp = state.range(8 + 7, 8); // state[0][1]
  state(8 + 7, 8) = state.range(40 + 7, 40);
  state(40 + 7, 40) = state.range(72 + 7, 72);
  state(72 + 7, 72) = state.range(104 + 7, 104);
  state(104 + 7, 104) = temp;
  
  // rotate row 2
  temp = state.range(16 + 7, 16);
  state(16 + 7, 16) = state.range(80 + 7, 80);
  state(80 + 7, 80) = temp;
  
  temp = state.range(48 + 7, 48);
  state(48 + 7, 48) = state.range(112 + 7, 112);
  state(112 + 7, 112) = temp;

  // rotate row 3
  temp = state.range(24 + 7, 24);
  state(24 + 7, 24) = state.range(120 + 7, 120);
  state(120 + 7, 120) = state.range(88 + 7, 88);
  state(88 + 7, 88) = state.range(56 + 7, 56);
  state(56 + 7, 56) = temp;
 
  return state;
}

static bit8_t xtime(bit8_t x) {
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

static state_t MixColumns(state_t state) {
//  #pragma HLS inline
  uint8_t i, c_off;
  bit8_t all, comb, s0c;
MIXCOL:
  for (i = 0; i < 4; i++) {
    #pragma HLS unroll
    c_off = i * 32;
    s0c = state.range(c_off + 7, c_off);
    all = state.range(c_off + 7, c_off) ^ state.range(c_off + 15, c_off + 8) 
          ^ state.range(c_off + 23, c_off + 16) ^ state.range(c_off + 31, c_off + 24);
    
    comb = state.range(c_off + 7, c_off) ^ state.range(c_off + 15, c_off + 8); 
    comb = xtime(comb);
    state(c_off + 7, c_off) = state.range(c_off + 7, c_off) ^ comb ^ all;
    
    comb = state.range(c_off + 15, c_off + 8) ^ state.range(c_off + 23, c_off + 16);
    comb = xtime(comb);
    state(c_off + 15, c_off + 8) = state.range(c_off + 15, c_off + 8) ^ comb ^ all;

    comb = state.range(c_off + 23, c_off + 16) ^ state.range(c_off + 31, c_off + 24);
    comb = xtime(comb);
    state(c_off + 23, c_off + 16) = state.range(c_off + 23, c_off + 16) ^ comb ^ all;

    comb = state.range(c_off + 31, c_off + 24) ^ s0c;
    comb = xtime(comb);
    state(c_off + 31, c_off + 24) = state.range(c_off + 31, c_off + 24) ^ comb ^ all;
  }
  return state;
}

static state_t Cipher(state_t state, roundkey_t roundKey) {
//  #pragma HLS inline
  uint8_t round = 0;
  state = AddRoundKey(0, state, roundKey);
  for (round = 1; round < Nr; round++) {
    state = SubBytes(state);
    state = ShiftRows(state);
    state = MixColumns(state);
    state = AddRoundKey(round, state, roundKey);
  }
  state = SubBytes(state);
  state = ShiftRows(state);
  state = AddRoundKey(Nr, state, roundKey);
  return state;
}

/*****************************************************************************/
/* Public functions:                                                         */
/*****************************************************************************/
void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
)
{
  int i, j, bi;
  bit32_t read;

  aes_key_t key;
  roundkey_t w;
  bit128_t iv, iv_enc, buf;

  // first read key
  for (i = 0; i < Nk; i++)
    key(i * 32 + 31, i * 32) = strm_in.read();
  
  keyExpansion(w, key);

  // read IV
  for (i = 0; i < Nb; i++)
    iv(i * 32 + 31, i * 32) = strm_in.read();

ENCR:
  for (i = 0, bi = BLOCKLEN; i < NUM_BLOCKS; i++, bi++) {
    #pragma HLS pipeline
    // read in block
  READ:
    for (j = 0; j < Nb; j++)
       buf(j * 32 + 31, j * 32) = strm_in.read();

    // encrypt IV
    iv_enc = Cipher(iv, w);

  INCR:
    for (bi = (BLOCKLEN - 1); bi >= 0; --bi) {
      // check overflow
      if (iv.range(bi * 8 + 7, bi * 8) == 255) {
        iv(bi * 8 + 7, bi * 8) = 0;
        continue;
      }
      iv(bi * 8 + 7, bi * 8) = iv.range(bi * 8 + 7, bi * 8) + 1;
      break;
    }
    
    buf ^= iv_enc;

  WRITE:
    for (j = 0; j < Nb; j++)
      strm_out.write(buf(j * 32 + 31, j * 32));
  }
}

