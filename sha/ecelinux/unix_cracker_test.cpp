#include <stdio.h>
#include <assert.h>
#include <hls_stream.h>
#include "typedefs.h"
//#include <stdlib.h> for crypt()
#include "unix_cracker.h"
// Check with echo -n "hello world" | sha512sum -t

extern void dut(hls::stream<ap_uint<32> >&, hls::stream<ap_uint<32> >&);

int main() {
  // Note: if using crypt, prepend with $6$
  const char salt[] = "8n./Hzqd";
  const char pass[] = "This is my password!";
  hls::stream<ap_uint<32> > in;
  hls::stream<ap_uint<32> > out;
  for (int i=0; i < strlen(pass); i++) {
    in.write(pass[i]);
  }
  in.write(0);
  dut(in, out);
  char hash[HASH_LEN+1];
  hash[HASH_LEN] = 0;
  for (int i=0; i < HASH_LEN; i++) {
    hash[i] = out.read();
  } 
  printf("Hash: %s\n", hash);
  return 0;
}
