#pragma once

#include <gmp.h>

template <typename T, int BIT_LEN>
T mpz_to_ap(mpz_t x) {
  T result(0);
  for (int i = 0; i < BIT_LEN; i++) {
    result.set(i, mpz_tstbit(x, i));
  }
  return result;
}

template <typename T, int BIT_LEN>
void ap_to_mpz(mpz_t out, T in) {
  mpz_set_si(out, 0);
  for (int i = 0; i < BIT_LEN; i++) {
    if (in[i]) {
      mpz_setbit(out, i);
    }
  }
}
