
#include <stdio.h>
#include <string.h>
#include "unix_cracker.h"

// Check with echo -n "hello world" | sha512sum -t
int main() {
  // Note: if using crypt, prepend with $6$
  const char salt[] = "8n./Hzqd";
  const char pass[] = "This is my password!";

  char hash[86];
  for (int i=0; i < 1; i++) {
    calc(hash, pass, strlen(pass), salt, strlen(salt));
  }
  printf("%s\n", hash);
    //out = crypt(pass, salt);

  // for (int i=0; i < 64; i++) {
  //   printf("%02x,", res.hash[i]);
  // }  // printf("\n");


}
