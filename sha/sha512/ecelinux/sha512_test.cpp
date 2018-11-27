#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>

#include "sha512.h"

SHA512Hash sha512(const void *data, uint64_t nbytes) {
  SHA512Hash curr = SHA512_INIT;
  uint8_t *cdata = (uint8_t*)data;

  uint64_t total = 0;
  while (total <= nbytes) {
    if (nbytes - total >= 128) {
      curr = hashBlock(curr, (uint64_t*)(cdata + total));
    } else {
      uint8_t last[128];
      memset(last, 0, sizeof(last));
      uint64_t remain = nbytes - total;
      memcpy(last, cdata + total, remain);
      last[remain] = 0x80;
      if (128 - (remain + 1) >= 2*sizeof(uint64_t)) { // We have room at the end
        *((uint64_t*)(&last[128-sizeof(uint64_t)])) = portable_htobe64(nbytes*8);
        curr = hashBlock(curr, (uint64_t*)last);
      } else { // Hash as is, then do last block
        curr = hashBlock(curr, (uint64_t*)last);
        memset(last, 0, sizeof(last));
        *((uint64_t*)(&last[128-sizeof(uint64_t)])) = portable_htobe64(nbytes*8);
        curr = hashBlock(curr, (uint64_t*)last);
      }
    }
    total += 128;
  }
  return curr;
}

int main() {
  const char s[] = "hello w";
  SHA512Hash target = sha512(s, strlen(s));

  for (int i=0; i < 8; i++) {
    printf("%" PRIx64 "\n", target.hash[i]);
  }
}
