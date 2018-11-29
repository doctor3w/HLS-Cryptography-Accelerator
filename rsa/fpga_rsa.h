#pragma once

#include "big_ap_int.h"
#include "rsa_config.h"
#include "hls_stream.h"
#include "bignum.h"

const int BITS_PER_DIGIT = 128;
typedef Bignum<2 * MAX_BIT_LEN / BITS_PER_DIGIT, BITS_PER_DIGIT> RsaBignum;
typedef ap_uint<MAX_BIT_LEN> RsaNum;
typedef ap_uint<32> bit32_t;

void dut(hls::stream<bit32_t>& strm_in, hls::stream<bit32_t>& strm_out);

RsaNum read_rsa_num(hls::stream<bit32_t>& in);
void write_rsa_num(RsaNum num, hls::stream<bit32_t>& out);

RsaNum fpga_powm(RsaNum base,
                               RsaNum exponent,
                               RsaNum modulus);
