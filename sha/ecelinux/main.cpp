#include "unix_cracker.h"
#include "typedefs.h"

#include <hls_stream.h>
#include <fstream>
#include <cstdio>
#include <iomanip>

using namespace std;

void dut(
    hls::stream<ap_uint<32> > &strm_in,
    hls::stream<ap_uint<32> > &strm_out
)
{
  uint8_t passwd[MAX_PWD_LEN+1] = {};
  uint8_t hash[HASH_LEN];
  int p, i;
  const uint8_t salt[MAX_SALT_LEN+1] = "8n./Hzqd";
  const int s = MAX_SALT_LEN;
  uint32_t passwd_bytes = 0, hash_bytes = 0;

  // Receive the password
  for (p=0; p < sizeof(passwd); p++) {
    if (p % 4 == 0) {
      passwd_bytes = strm_in.read();
    } else {
      passwd_bytes >>= 8;
    }
    passwd[p] = passwd_bytes;
    if (!passwd[p]) break;
  }

  // Compute the hash
	calc(hash, passwd, p, salt, s);

  //Send back the hash
  for (int i = 0; i < HASH_LEN; i++) {
    if (i % 4 == 0 && i >= 4) {
      strm_out.write(hash_bytes);
      hash_bytes = 0;
    } else if (i % 4 != 0) {
      hash_bytes >>= 8;
    }
    hash_bytes |= (uint32_t)hash[i] << 24;
  }
 
  hash_bytes >>= 16;
  strm_out.write(hash_bytes);
}
