#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"
#include "timer.h"

static void readBytes(char* fn, uint8_t* out, int n) {
  FILE* in;
  in = fopen(fn, "r");
  if (in == NULL) {
    printf("Failed reading...\n");
    return;
  }

  unsigned int i;
  char ch;
  for (i = 0; i < n; i++) {
    ch = fgetc(in);
    out[i] = ch;
  }

  fclose(in);
}

static void test_xcrypt_ctr() {

  char out[Nb * NUM_BLOCKS * 4];
  uint8_t in[Nb * NUM_BLOCKS * 4];
  uint8_t outCorrect[Nb * NUM_BLOCKS * 4];

  char correct = 0;

  Timer timer("aes encr");
  timer.start();

#if defined(AES_256) && AES_256 == 1

  uint8_t key[32] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
  };
  const char* fn = "./data/aes256_out";

#elif defined(AES_192) && AES_192 == 1

  uint8_t key[24] = {
    0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
  };
  const char* fn = "./data/aes192_out";

#elif defined(AES_128) && AES_128 == 1

  uint8_t key[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
  };
  const char* fn = "./data/aes128_out";

#endif

  uint8_t iv[16] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
  };

  // Read in data
  readBytes("./data/8000bytes", in, Nb * NUM_BLOCKS * 4);
  readBytes(fn, outCorrect, Nb * NUM_BLOCKS * 4);

  // Do computation
  struct AES_ctx ctx;
  AES_init_ctx_iv(&ctx, key, iv);
  AES_CTR_xcrypt_buffer(&ctx, in, Nbytes);

  if (0 == memcmp((char*) out, (char*) in, Nbytes)) {
    correct = 1;
  }

  timer.stop();

  // Calculate accuracy
  std::cout << "Success: " << (correct ? "true" : "false") << std::endl;
}

int main(void) {
#if defined(AES256)
  printf("\nTesting AES256\n\n");
#elif defined(AES192)
  printf("\nTesting AES192\n\n");
#elif defined(AES128)
  printf("\nTesting AES128\n\n");
#else
  printf("You need to specify a symbol between AES128, AES192 or AES256. Exiting");
  return -1;
#endif

  test_xcrypt_ctr();
  return 0;
}
