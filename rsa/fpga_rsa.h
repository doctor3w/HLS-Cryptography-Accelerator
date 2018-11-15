#pragma once

#include "big_ap_int.h"
#include "rsa_config.h"

ap_uint<MAX_BIT_LEN> fpga_powm(ap_uint<MAX_BIT_LEN> base,
                               ap_uint<MAX_BIT_LEN> exponent,
                               ap_uint<MAX_BIT_LEN> modulus);
