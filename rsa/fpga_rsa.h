#pragma once

#include "big_ap_int.h"
#include "bignum.h"
#include "hls_stream.h"
#include "rsa_config.h"

const int BITS_PER_DIGIT = 64;
const int INT32S_PER_DIGIT = BITS_PER_DIGIT / 32;
typedef Bignum<2 * MAX_BIT_LEN / BITS_PER_DIGIT, BITS_PER_DIGIT> RsaBignum;
typedef ap_uint<32> bit32_t;

void dut(hls::stream<bit32_t>& strm_in, hls::stream<bit32_t>& strm_out);

RsaBignum read_rsa_num(hls::stream<bit32_t>& in);
void write_rsa_num(RsaBignum num, hls::stream<bit32_t>& out);

RsaBignum fpga_powm(RsaBignum base, RsaBignum exponent, RsaBignum modulus);
