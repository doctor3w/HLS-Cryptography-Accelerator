//===========================================================================
// typedefs.h
//===========================================================================
// @brief: This header defines the shorthand of several ap_uint data types.

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define LOCAL 0
#if LOCAL == 1
typedef uint32_t bit32_t;
#else
#include "ap_int.h"
typedef ap_uint<8> bit8_t;
typedef ap_uint<32> bit32_t;
typedef ap_uint<128> bit128_t;
#endif

#endif
