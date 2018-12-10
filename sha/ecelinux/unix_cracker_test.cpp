#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <hls_stream.h>
#include <ap_int.h>

#include "unix_cracker.h"

extern void dut(hls::stream<ap_uint<32> >&, hls::stream<ap_uint<32> >&);

int main(int argc, char** argv)
{
  hls::stream<ap_uint<32> > in("in");
  hls::stream<ap_uint<32> > out("out");

  // Open the passwords file for reading
  FILE* fp;
  size_t len = 0;
  char* pass = NULL;
  fp = fopen("/home/aik49/SuperAwesomeFastCryptoHLSAcceleratorThing/sha/ecelinux/passwords.txt", "r");
  if (fp == NULL) {
    printf("Error reading the passwords file!\n");
    exit(-1);
  }

  char hash[86+1] = {};
  uint32_t passwd_bytes, hash_bytes;
  struct timespec t1, t2;
  int num_passwords = 0, i;
  float total_time = 0;
  while ((getline(&pass, &len, fp) != -1) && num_passwords < 1) {
    if (strlen(pass) > MAX_PWD_LEN) continue;
 
    // Remove the newline at the end
    pass[strlen(pass)-1] = 0;

    printf("%s -> ", pass);
    clock_gettime(CLOCK_MONOTONIC, &t1);
 
    // Send all the password chars that can be sent in chunks of 4
    passwd_bytes = 0;
    for (i = 0; i < strlen(pass)+1; i++) {
      if (i % 4 == 0 && i >= 4) {
        in.write(passwd_bytes);
        passwd_bytes = 0;
      } else if (i % 4 != 0) {
        passwd_bytes >>= 8;
      }
      passwd_bytes |= (uint32_t)pass[i] << 24;
    }

    // Send remaining bytes
    passwd_bytes >>= 8 * (i % 4);
    in.write(passwd_bytes);

    dut(in, out);

    // Read the hash from the fpga
    for (i = 0; i < HASH_LEN; i++) {
      if (i % 4 == 0) {
        hash_bytes = out.read();
      } else {
        hash_bytes >>= 8;
      }
      hash[i] = hash_bytes;
    }

    clock_gettime(CLOCK_MONOTONIC, &t2);
    total_time += float(t2.tv_sec - t1.tv_sec) + (float(t2.tv_nsec)*1e-9 - float(t1.tv_nsec)*1e-9);
    printf("%s\n", hash);
    num_passwords++;
  }

  printf("Average time %fs\n", total_time/num_passwords);
  printf("Passwords/second %f\n", 1/(total_time/num_passwords));

  return 0;
}
