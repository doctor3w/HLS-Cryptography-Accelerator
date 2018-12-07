#pragma once

#include <stdint.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))



void memcpy_u8(uint8_t *dest, const uint8_t *src, int nbytes);


void memset_u8(uint8_t *dest, uint8_t val, int nbytes);

// Unroll completely
uint64_t read64clear(uint8_t *arr, int sidx);
