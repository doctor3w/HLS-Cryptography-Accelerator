#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<math.h>
#include<assert.h>
#include <inttypes.h>
#include <string.h>

#include<iostream>
#include<fstream>
#include "unix_cracker.h"

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


  //--------------------------------------------------------------------
  // Send data to accelerator
  //--------------------------------------------------------------------
  const char salt[MAX_SALT_LEN] = "8n./Hzqd";
  const char pass[MAX_PWD_LEN] = "This is my password!";

  // Fix this later
  uint32_t salt32[MAX_SALT_LEN];
  uint32_t pass32[MAX_PWD_LEN];

  for (int i=0; i < MAX_SALT_LEN; i++) salt32[i] = salt[i];
  for (int i=0; i < MAX_PWD_LEN; i++) pass32[i] = pass[i];

  // Send salt
  int nbytes;
  nbytes = write (fdw, (void*)&salt, strlen(salt) + 1);
  assert (nbytes == strlen(salt) + 1);

  // Send pass
  nbytes = write (fdw, (void*)&pass, strlen(pass) + 1);
  assert (nbytes == strlen(pass) + 1);


  //--------------------------------------------------------------------
  // Receive data from the accelerator
  //--------------------------------------------------------------------
  uint32_t output[86 + 1] = {};
  nbytes = read (fdr, (void*)&output, 86);
  assert (nbytes == 86);

  // TODO: fix this later
  char out[86+1];
  for (int i=0; i < 86; i++) out[i] = output[i];

  printf("Hash: %s\n", out);

  return 0;
}
