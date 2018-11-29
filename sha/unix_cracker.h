#pragma once

#include <stdint.h>

#define MAX_PWD_LEN 32
#define MAX_SALT_LEN 32

#define HASH_LEN 86

void calc(char hash[HASH_LEN], const char pwd[MAX_PWD_LEN], const uint8_t pwlen,
        const char salt[MAX_SALT_LEN], const uint8_t slen, int nrounds=5000);
