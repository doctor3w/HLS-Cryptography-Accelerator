#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

#define MAX_BYTES 128
#define MAX_BIT_LEN (MAX_BYTES * CHAR_BIT)

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
    filled += getrandom(buf + filled, len - filled, 0);
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
}

void print_hex(char* arr, int len) {
  for (int i = 0; i < len; i++) {
    printf("%02x", (unsigned char)arr[i]);
  }
}

void generate_keys(private_key_t* ku, public_key_t* kp, int bytes) {
  mpz_t phi;
  mpz_init(phi);
  mpz_t two;
  mpz_init_set_si(two, 2);

  mpz_t p, q;
  mpz_inits(p, q);

  do {
    probable_prime(p, bytes * CHAR_BIT / 2);
    probable_prime(q, bytes * CHAR_BIT / 2);
    mpz_mul(ku->n, p, q);

    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);
  } while (mpz_cmp_si(phi, 2) <= 0);
  mpz_set(kp->n, ku->n);

  mpz_t gcd;
  mpz_init(gcd);
  do {
    random_mpz(kp->e, two, phi);
    mpz_gcd(gcd, kp->e, phi);
  } while (mpz_cmp_si(gcd, 1) != 0);
  mpz_clear(gcd);

  mpz_invert(ku->d, kp->e, phi);

  mpz_clear(phi);
  mpz_clear(two);

  ku->bytes = bytes;
  kp->bytes = bytes;
}

void block_encrypt(mpz_t C, mpz_t M, public_key_t kp) {
  mpz_powm(C, M, kp.e, kp.n);
  return;
}

int encrypt(char* cipher, const char* message, int length, public_key_t kp,
            char use_cbc) {
  size_t loc = 0;
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

  mpz_t m;
  mpz_init(m);
  mpz_t c;
  mpz_init(c);

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
      for (int i = 0; i < chunk_size; i++) {
        buf[i] ^= iv[i];
      }
    }

    mpz_import(m, chunk_size, 1, sizeof(buf[0]), 0, 0, buf);
    block_encrypt(c, m, kp);

    int off = loc + (bytes - (mpz_sizeinbase(c, 2) + 8 - 1) / 8);
    mpz_export(cipher + off, NULL, 1, sizeof(char), 0, 0, c);
    memcpy(iv, cipher + loc, bytes);
    loc += bytes;
  }
  mpz_clear(m);
  mpz_clear(c);
  return loc;
}

void block_decrypt(mpz_t M, mpz_t C, private_key_t ku) {
  mpz_powm(M, C, ku.d, ku.n);
  return;
}

int decrypt(char* message, const char* cipher, int length, private_key_t ku,
            char use_cbc) {
  size_t loc = 0;
  int bytes = ku.bytes;
  // 1 byte less for length, 1 byte less for space
  int chunk_size = bytes - 1;
  int bytes_per_chunk = chunk_size - 2;

  char iv[MAX_BYTES];
  if (use_cbc) {
    memcpy(iv, cipher, bytes);
    loc += bytes;
  }

  mpz_t m;
  mpz_init(m);
  mpz_t c;
  mpz_init(c);

  int msg_idx = 0;
  for (; loc < length; loc += bytes) {
    char buf[MAX_BYTES];
    mpz_import(c, bytes, 1, sizeof(char), 0, 0, cipher + loc);
    block_decrypt(m, c, ku);
    memset(buf, 0, chunk_size);
    int off = chunk_size - (mpz_sizeinbase(m, 2) + 8 - 1) / 8;
    mpz_export(buf + off, NULL, 1, sizeof(char), 0, 0, m);

    if (use_cbc) {
      for (int i = 0; i < bytes; i++) {
        buf[i] ^= iv[i];
      }
      memcpy(iv, cipher + loc, bytes);
    }

    int len = buf[bytes_per_chunk];
    memcpy(message + msg_idx, buf, len);
    msg_idx += len;
  }
  return msg_idx;
}

void print_buf(char* buf, size_t len) {
  for (int i = 0; i < len; i++) {
    printf("%02hhX", (int)buf[i]);
  }
  printf("\n");
}

int main() {
  mpz_t M;
  mpz_init(M);
  mpz_t C;
  mpz_init(C);
  mpz_t DC;
  mpz_init(DC);
  private_key_t ku;
  public_key_t kp;

  // Initialize public key
  mpz_init(kp.n);
  mpz_init(kp.e);
  // Initialize private key
  mpz_init(ku.n);
  mpz_init(ku.d);

  generate_keys(&ku, &kp, 128);
  printf("---------------Private Key-----------------");
  printf("kp.n is\n%s\n", mpz_get_str(NULL, 16, kp.n));
  printf("kp.e is\n%s\n", mpz_get_str(NULL, 16, kp.e));
  printf("---------------Public Key------------------");
  printf("ku.n is\n%s\n", mpz_get_str(NULL, 16, ku.n));
  printf("ku.d is\n%s\n", mpz_get_str(NULL, 16, ku.d));

  printf("bitlen(n) = %lu\n", mpz_sizeinbase(kp.n, 2));

  char buf[127];
  fill_random(buf, sizeof(buf));

  mpz_import(M, sizeof(buf), 1, sizeof(buf[0]), 0, 0, buf);
  printf("original: \n%s\n\n", mpz_get_str(NULL, 62, M));
  block_encrypt(C, M, kp);
  printf("encrypted is\n%s\n", mpz_get_str(NULL, 62, C));
  block_decrypt(DC, C, ku);
  printf("decrypted is\n%s\n", mpz_get_str(NULL, 62, DC));

  printf("------------------");
  printf("\n\n\n");
  const char* in =
      "hi my name is bob and I am super long. I am a really long really cool "
      "message that will require multiple blocks. Haha take t";
  char out[128 * 50];
  int len = encrypt(out, in, strlen(in) + 1, kp, 1);
  printf("Encrypted: %d\n", len);
  print_buf(out, len);
  char result[128 * 50];
  len = decrypt(result, out, len, ku, 1);
  printf("Decrypted: %d\n", len);
  printf("%s\n", result);
}
