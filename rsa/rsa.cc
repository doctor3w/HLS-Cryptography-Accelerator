#include <gmp.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "fpga_rsa.h"
#include "mpz_adapters.h"
//#include <sys/random.h>
#include "rsa_config.h"

typedef struct {
  mpz_t n;
  mpz_t e;
  int bytes;
} public_key_t;

typedef struct {
  mpz_t n;
  mpz_t d;
  int bytes;
} private_key_t;

void fill_random(char* buf, size_t len) {
  size_t filled = 0;
  while (filled < len) {
    buf[filled] = rand() & 0xFF;
    filled += 1;
    // filled += getrandom(buf + filled, len - filled, 0);
  }
}

void random_mpz_bits(mpz_t out, int bits) {
  char buf[MAX_BYTES];
  int bytes = bits / CHAR_BIT + (bits % CHAR_BIT == 0 ? 0 : 1);
  fill_random(buf, bytes);
  int good_top_bits = bits % CHAR_BIT;
  if (good_top_bits == 0) {
    good_top_bits = CHAR_BIT;
  }
  char high_mask = ~(-1 << good_top_bits);
  buf[0] &= high_mask;
  mpz_import(out, bytes, 1, sizeof(buf[0]), 0, 0, buf);
}

void probable_prime(mpz_t x, int bits) {
  mpz_t temp;
  mpz_init(temp);
  random_mpz_bits(temp, bits);
  // Set the top bit
  mpz_setbit(temp, bits - 1);
  // Set the bottom bit to 1 so it is odd
  mpz_setbit(temp, 0);

  mpz_nextprime(x, temp);
  mpz_clear(temp);
}

// Generates a random value from the range [min, max)
void random_mpz(mpz_t out, mpz_t min, mpz_t max) {
  mpz_t interval_size;
  mpz_init(interval_size);
  mpz_sub(interval_size, max, min);
  size_t bits = mpz_sizeinbase(interval_size, 2);
  do {
    random_mpz_bits(out, bits);
  } while (mpz_cmp(interval_size, out) <= 0);
  mpz_add(out, out, min);
  mpz_clear(interval_size);
}

void print_hex(const char* arr, int len) {
  for (int i = 0; i < len; i++) {
    printf("%02x", (unsigned char)arr[i]);
  }
}

void private_key_init(private_key_t* ku) { mpz_inits(ku->n, ku->d, NULL); }

void public_key_init(public_key_t* kp) { mpz_inits(kp->n, kp->e, NULL); }

void private_key_clear(private_key_t* ku) { mpz_clears(ku->n, ku->d, NULL); }

void public_key_clear(public_key_t* kp) { mpz_clears(kp->n, kp->e, NULL); }

void generate_keys(private_key_t* ku, public_key_t* kp, int bytes) {
  mpz_set_si(kp->e, STANDARD_E_VALUE);

  mpz_t phi, p, q, gcd;
  mpz_inits(phi, p, q, gcd, NULL);
  do {
    probable_prime(p, bytes * CHAR_BIT / 2);
    probable_prime(q, bytes * CHAR_BIT / 2);
    mpz_mul(ku->n, p, q);

    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);
    // e < phi && e > 2 must hold
    // e is always > 2 though
    mpz_gcd(gcd, kp->e, phi);
    // gcd(e, phi) must not be 1
  } while (mpz_cmp(kp->e, phi) >= 0 || mpz_cmp_si(gcd, 1) != 0);

  mpz_set(kp->n, ku->n);
  mpz_invert(ku->d, kp->e, phi);
  mpz_clears(phi, p, q, gcd, NULL);

  ku->bytes = bytes;
  kp->bytes = bytes;
}

void xor_array(char* a, const char* b, int len) {
  for (int i = 0; i < len; i++) {
    a[i] ^= b[i];
  }
}

void fpga_rsa_block_adapter(mpz_t out, mpz_t data, mpz_t n, mpz_t e) {
  ap_uint<MAX_BIT_LEN> data_ap = mpz_to_ap<MAX_BIT_LEN>(data);
  ap_uint<MAX_BIT_LEN> n_ap = mpz_to_ap<MAX_BIT_LEN>(n);
  ap_uint<MAX_BIT_LEN> e_ap = mpz_to_ap<MAX_BIT_LEN>(e);

  ap_to_mpz(out, fpga_powm(data_ap, e_ap, n_ap));
}

void block_encrypt(mpz_t c, mpz_t m, public_key_t kp) {
  // mpz_powm(c, m, kp.e, kp.n);
  fpga_rsa_block_adapter(c, m, kp.n, kp.e);
}

void block_decrypt(mpz_t m, mpz_t c, private_key_t ku) {
  // mpz_powm(m, c, ku.d, ku.n);
  fpga_rsa_block_adapter(m, c, ku.n, ku.d);
}

int encrypt(char* cipher, int cipher_len, const char* message, int length,
            public_key_t kp, char use_cbc) {
  int loc = 0;
  int bytes = kp.bytes;
  // 1 byte less for length, 1 byte less for space
  int chunk_size = bytes - 1;
  int bytes_per_chunk = chunk_size - 2;

  char iv[MAX_BYTES];
  if (use_cbc) {
    fill_random(iv, bytes);
    memcpy(cipher, iv, bytes);
    loc += bytes;
  }

  mpz_t m, c;
  mpz_inits(m, c, NULL);

  for (int x = 0; x < length; x += bytes_per_chunk) {
    char buf[MAX_BYTES];
    int size = bytes_per_chunk;
    if (x + bytes_per_chunk >= length) {
      size = length - x;
    }
    // random padding with final length byte
    memcpy(buf, message + x, size);
    fill_random(buf + size, bytes_per_chunk - size);
    buf[bytes_per_chunk] = (char)size;

    if (use_cbc) {
      xor_array(buf, iv, chunk_size);
    }

    mpz_import(m, chunk_size, 1, sizeof(buf[0]), 0, 0, buf);
    block_encrypt(c, m, kp);

    if (cipher_len < loc + bytes) {
      loc = -1;
      break;
    }
    int off = loc + (bytes - (mpz_sizeinbase(c, 2) + 8 - 1) / 8);
    mpz_export(cipher + off, NULL, 1, sizeof(char), 0, 0, c);
    memcpy(iv, cipher + loc, bytes);
    loc += bytes;
  }

  mpz_clears(m, c, NULL);
  return loc;
}

int decrypt(char* message, int message_len, const char* cipher, int length,
            private_key_t ku, char use_cbc) {
  int loc = 0;
  int bytes = ku.bytes;
  // 1 byte less for length, 1 byte less for space
  int chunk_size = bytes - 1;
  int bytes_per_chunk = chunk_size - 2;

  char iv[MAX_BYTES];
  if (use_cbc) {
    memcpy(iv, cipher, bytes);
    loc += bytes;
  }

  mpz_t m, c;
  mpz_inits(m, c, NULL);

  int msg_idx = 0;
  for (; loc < length; loc += bytes) {
    char buf[MAX_BYTES];
    mpz_import(c, bytes, 1, sizeof(char), 0, 0, cipher + loc);
    block_decrypt(m, c, ku);
    memset(buf, 0, chunk_size);
    int off = chunk_size - (mpz_sizeinbase(m, 2) + 8 - 1) / 8;
    mpz_export(buf + off, NULL, 1, sizeof(char), 0, 0, m);

    if (use_cbc) {
      xor_array(buf, iv, bytes);
      memcpy(iv, cipher + loc, bytes);
    }

    int len = buf[bytes_per_chunk];
    if (message_len < msg_idx + len) {
      msg_idx = -1;
      break;
    }
    memcpy(message + msg_idx, buf, len);
    msg_idx += len;
  }

  mpz_clears(m, c, NULL);
  return msg_idx;
}

void print_buf(char* buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("%02hhX", (int)buf[i]);
  }
  printf("\n");
}

int main() {
  private_key_t ku;
  public_key_t kp;
  private_key_init(&ku);
  public_key_init(&kp);

  generate_keys(&ku, &kp, MAX_BYTES);
  printf("---------------Private Key----------------\n");
  printf("n = %s\n", mpz_get_str(NULL, 16, kp.n));
  printf("e = %s\n", mpz_get_str(NULL, 16, kp.e));
  printf("---------------Public Key------------------\n");
  printf("n = %s\n", mpz_get_str(NULL, 16, ku.n));
  printf("d = %s\n", mpz_get_str(NULL, 16, ku.d));
  printf("-------------------------------------------\n");
  printf("bitlen(n) = %zu\n", mpz_sizeinbase(kp.n, 2));
  printf("-------------------------------------------\n");

  const char* in =
      "hi my name is bob and I am super long. I am a really long really cool "
      "message that will require multiple blocks. Haha take t";
  char out[128 * 50];
  int len = encrypt(out, sizeof(out), in, strlen(in) + 1, kp, 1);
  if (len == -1) {
    printf("encryption failed\n");
    return 1;
  }
  printf("Encrypted: %d\n", len);
  print_buf(out, len);
  char result[125];
  len = decrypt(result, sizeof(result), out, len, ku, 1);
  if (len == -1) {
    printf("decryption failed\n");
    return 1;
  }
  printf("Decrypted: %d\n", len);
  printf("%s\n", result);
}
