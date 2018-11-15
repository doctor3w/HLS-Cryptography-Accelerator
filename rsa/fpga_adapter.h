#pragma once

#include "rsa_config.h"
#include <ap_int.h>
#include "mpz_adapters.h"
#include "fpga_rsa.h"

template <int BIT_LEN>
void fpga_rsa_block_adapter(mpz_t out, mpz_t data, mpz_t n, mpz_t e) {
  ap_uint<BIT_LEN> data_ap = mpz_to_ap<BIT_LEN>(data);
  ap_uint<BIT_LEN> n_ap = mpz_to_ap<BIT_LEN>(n);
  ap_uint<BIT_LEN> e_ap = mpz_to_ap<BIT_LEN>(e);

  ap_to_mpz(out, fpga_rsa_block(data_ap, n_ap, e_ap));
}
