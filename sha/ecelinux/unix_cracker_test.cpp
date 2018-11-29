#include <stdio.h>
#include <assert.h>
#include <hls_stream.h>
//#include <stdlib.h> for crypt()
#include "unix_cracker.h"
// Check with echo -n "hello world" | sha512sum -t

void dut(hls::stream<uint32_t>&, hls::stream<uint32_t>&);

int main() {
  // Note: if using crypt, prepend with $6$
  const char salt[] = "8n./Hzqd";
  const char pass[] = "This is my password!";
  hls::stream<uint32_t> in;
  hls::stream<uint32_t> out;
  in.write(0);
  dut(in, out);
  return 0;
}
