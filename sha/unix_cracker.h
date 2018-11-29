#pragma once

#include <stdint.h>

#define MAX_PWD_LEN 32
#define MAX_SALT_LEN 32

void calc(char hash[86], const char pwd[MAX_PWD_LEN], const uint8_t pwlen,
        const char salt[MAX_SALT_LEN], const uint8_t slen, int nrounds=5000);
