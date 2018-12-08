#define _XOPEN_SOURCE
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

int main() {
  std::ifstream passFile("passwords.txt");
  std::string pass;
  char* hash;
  const std::string salt = "$6$8n./Hzqd";
  int num_passwords = 0;
  struct timespec t1, t2;
  float total_time;
 
  while(passFile >> pass && num_passwords < 100) {
    printf("%s -> ", pass.c_str());
    clock_gettime(CLOCK_MONOTONIC, &t1);
    hash = crypt(pass.c_str(), salt.c_str());
    clock_gettime(CLOCK_MONOTONIC, &t2);
    total_time += float(t2.tv_sec - t1.tv_sec) + (float(t2.tv_nsec)*1e-9 - float(t1.tv_nsec)*1e-9);
    printf("%s\n", hash);
    num_passwords++;
  }

  printf("Average time %fs\n", total_time/num_passwords);
  printf("Passwords/second %f\n", 1/(total_time/num_passwords));
}
