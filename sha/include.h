#pragma once


#include <stdint.h>


extern "C" {
  // arg 1 : pointer to input data
  // arg 2 : pointer to digest
  // arg 3 : Num blocks
  void sha256_sse4(void *input_data, uint32_t digest[8], uint64_t num_blks);

  void sha256_avx(void *input_data, uint32_t digest[8], uint64_t num_blks);

  void sha256_rorx(void *input_data, uint32_t digest[8], uint64_t num_blks);

  void sha256_rorx_x8ms(void *input_data, uint32_t digest[8], uint64_t num_blks);
}
