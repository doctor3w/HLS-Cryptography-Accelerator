//==========================================================================
// bnn.cpp
//==========================================================================
// @brief: A convolution kernel for CNN on digit recognition

#include "unix_cracker.h"
#include "typedefs.h"

#include <hls_stream.h>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(
    hls::stream<ap_uint<32> > &strm_in,
    hls::stream<ap_uint<32> > &strm_out
)
{
	char passwd[MAX_PWD_LEN];
  char salt[MAX_SALT_LEN];
  int s,p;

  // First read in the salt
  for (int s=0; s < MAX_SALT_LEN; s++) {
    salt[s] = strm_in.read();
    if (!salt[s]) break;
  }

  // Read in pass
  for (int p=0; p < MAX_PWD_LEN; p++) {
    passwd[p] = strm_in.read();
    if (!passwd[p]) break;
  }

  char hash[HASH_LEN];

  // Compute the hash
	calc(hash, passwd, p, salt, s);

  // Write the result out
	for (int i=0; i < HASH_LEN; i++) {
		strm_out.write(hash[i]);
  }

}
