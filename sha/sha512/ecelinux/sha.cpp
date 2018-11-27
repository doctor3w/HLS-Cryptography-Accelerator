#include "sha512.h"

#include <hls_stream.h>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
)
{	
	uint64_t block[16];
	for (int i=0; i < 16; i++) {
		block[i] = strm_in.read();
	}

   SHA512Hash init = SHA512_INIT;
   SHA512Hash test = hashBlock(init, block);
	
	for (int i = 0; i < 8; i++) {
  	strm_out.write(test.hash[i]);
  }
}
