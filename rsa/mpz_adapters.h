#pragma once

#include <gmp.h>
#include <ap_int.h>

template <int BIT_LEN>
ap_uint<BIT_LEN> mpz_to_ap(mpz_t x) {
  ap_uint<BIT_LEN> result = 0;
  for (int i = 0; i < BIT_LEN; i++) {
    result[i] = mpz_tstbit(x, i);
  }
  return result;
}

template <int BIT_LEN>
void ap_to_mpz(mpz_t out, ap_uint<BIT_LEN> in) {
  mpz_set_si(out, 0);
  for (int i = 0; i < BIT_LEN; i++) {
    if (in[i]) {
      mpz_setbit(out, i);
    }
  }
}

