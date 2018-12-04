//=========================================================================
// host.cpp
//=========================================================================
// @brief: testbench for AES application

#include <iostream>
#include <fstream>
#include "aes.h"
#include "timer.h"

using namespace std;

//------------------------------------------------------------------------
// Helper function for reading images and labels
//------------------------------------------------------------------------

static void readBytes(const char* fn, uint8_t* out, int n) {
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

static void printBytes(uint8_t* x, int l) {
    printf("{\n");
    for (int i = 0; i < l; i ++) {
        printf("0x%02x", x[i]);
        printf(i == l-1 ? "\n" : (i % 8 == 7 ? ",\n" : ", "));
    }
    printf("}\n");
}

//------------------------------------------------------------------------
// AES testbench
//------------------------------------------------------------------------

int main(){
  // HLS streams for communicating with the cordic block
  int fdr = open("/dev/xillybus_read_32", O_RDONLY);
  int fdw = open("/dev/xillybus_write_32", O_WRONLY);

  // Check that the channels are correctly opened
  if ((fdr < 0) || (fdw < 0)) {
    fprintf (stderr, "Failed to open Xillybus device channels\n");
    exit(-1);
  }

  char out[Nb * NUM_BLOCKS * 4];
  uint8_t in[Nb * NUM_BLOCKS * 4];
  uint8_t outCorrect[Nb * NUM_BLOCKS * 4];

  char correct = 0;
  
  Timer timer("aes encr");
  timer.start();

#if defined(AES_256) && AES_256 == 1
  uint8_t key[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
  const char* fn = "./data/aes256_out";
#elif defined(AES_192) && AES_192 == 1
  uint8_t key[24] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
  const char* fn = "./data/aes192_out";
#elif defined(AES_128) && AES_128 == 1
  uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
  const char* fn = "./data/aes128_out";
#endif  
 
  uint8_t iv[16] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
  readBytes("./data/8000bytes", in, Nb * NUM_BLOCKS * 4);
  readBytes(fn, outCorrect, Nb * NUM_BLOCKS * 4);

  int i, j;
  
  bit32_t wr; 
  uint32_t* keyD = (uint32_t*)key;
  for (i = 0; i < Nk; i++) {
    wr = keyD[i];
    write(fdw, (void*)&wr, sizeof(wr));
  } 

  uint32_t* ivD = (uint32_t*)iv;
  for (i = 0; i < Nb; i++) {
    wr = ivD[i];
    write(fdw, (void*)&wr, sizeof(wr));
  }

  uint32_t* inD = (uint32_t*)in;
  for (i = 0; i < Nb * NUM_BLOCKS; i++) {
    wr = inD[i];
    write(fdw, (void*)&wr, sizeof(wr));
  }
  
  dut(aes_in, aes_out);

  bit32_t rd;
  int nbytes;
  for (i = 0; i < Nb * NUM_BLOCKS; i++) {
    nbytes = read(fdr, (void*)&rd, sizeof(rd));
    out[i*4 + 0] = rd.range(7, 0);
    out[i*4 + 1] = rd.range(15, 8);
    out[i*4 + 2] = rd.range(23, 16);
    out[i*4 + 3] = rd.range(31, 24);
  }

  if (0 == memcmp((char*)outCorrect, (char*)out, BLOCKLEN * NUM_BLOCKS)) {
    correct = 1;
  }

  timer.stop();

  // Calculate accuracy
  std::cout << "Success: " << (correct ? "true" : "false") << std::endl;
  
  return 0;
}
