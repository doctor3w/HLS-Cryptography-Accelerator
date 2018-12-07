#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>

#define MAX_PWD_LEN 20
#define MAX_SALT_LEN 8
#define HASH_LEN 86

//--------------------------------------
// main function
//--------------------------------------
int main(int argc, char** argv)
{
  // Open channels to the FPGA board.
  // These channels appear as files to the Linux OS
  int fdr = open("/dev/xillybus_read_32", O_RDONLY);
  int fdw = open("/dev/xillybus_write_32", O_WRONLY);

  // Check that the channels are correctly opened
  if ((fdr < 0) || (fdw < 0)) {
    fprintf (stderr, "Failed to open Xillybus device channels\n");
    exit(-1);
  }

  // Open the passwords file for reading
  FILE* fp;
  size_t len = 0;
  fp = fopen("passwords.txt", "r");
  if (fp == NULL) {
    printf("Error reading the passwords file!\n");
    exit(-1);
  }

  // Strings for the salt and password
  const char salt[MAX_SALT_LEN+1] = "8n./Hzqd";
  char* pass = NULL;

  // These are necessary because the xillybus devices operate at the 32b granularity
  uint32_t salt32[MAX_SALT_LEN+1];
  uint32_t pass32[MAX_PWD_LEN+1];

  // Copy the salt string into one of the 32b arrays, pass is copied later
  for (int i=0; i < strlen(salt) + 1; i++) salt32[i] = salt[i];

  // Array for storing the hash -- since the xillybus device sends 32b at a time we 
  // need to store 4 times the hash length
  char out[86*4+1] = {};

  struct timespec t1, t2;
  int nbytes, num_passwords = 0;
  float total_time = 0;
  while ((getline(&pass, &len, fp) != -1) && num_passwords < 100) {
    if (strlen(pass) > MAX_PWD_LEN) continue;

    // Copy the password into the 32b array for sending
    pass[strlen(pass)-1] = '\0';
    for (int i=0; i < strlen(pass) + 1; i++) pass32[i] = pass[i];

    printf("%s -> ", pass);
    //--------------------------------------------------------------------
    // Send data to accelerator
    //--------------------------------------------------------------------    
    // Send salt
    clock_gettime(CLOCK_MONOTONIC, &t1);
    nbytes = write (fdw, (void*)&salt32, sizeof(uint32_t)*(strlen(salt)+1));
    assert (nbytes == sizeof(uint32_t)*(strlen(salt)+1));

    // Send pass
    nbytes = write (fdw, (void*)&pass32, sizeof(uint32_t)*(strlen(pass)+1));
    assert (nbytes == sizeof(uint32_t)*(strlen(pass)+1));
    
    //--------------------------------------------------------------------
    // Receive data from the accelerator
    //--------------------------------------------------------------------
    // Read the data from the fpga
    nbytes = 0;
    while (nbytes < 86*4) {
      nbytes += read (fdr, (void*)(out+nbytes), (86*4)-nbytes);
    }
    assert (nbytes == 86*4);

    clock_gettime(CLOCK_MONOTONIC, &t2);
    total_time += float(t2.tv_sec - t1.tv_sec) + (float(t2.tv_nsec)*1e-9 - float(t1.tv_nsec)*1e-9);

    // Copy the non-zero bytes into the first 86 positions of the array
    for(int i = 0; i < 86; i++) out[i] = out[i*4];

    printf(" %s\n", out);
    num_passwords++;
  }

  printf("Average time %fs\n", total_time/num_passwords);
  printf("Passwords/second %f\n", 1/(total_time/num_passwords));

  return 0;
}
