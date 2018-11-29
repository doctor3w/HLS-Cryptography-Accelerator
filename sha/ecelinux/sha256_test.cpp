#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "sha256.h"


SHA256Hash sha256(const void *data, uint64_t nbytes) {
  SHA256Hash curr = SHA256_INIT;
  uint8_t *cdata = (uint8_t*)data;

  uint64_t total = 0;
  while (total <= nbytes) {
    if (nbytes - total >= 64) {
      curr = hashBlock(curr, (uint32_t*)(cdata + total));
    } else {
      uint8_t last[64];
      memset(last, 0, sizeof(last));
      uint64_t remain = nbytes - total;
      memcpy(last, cdata + total, remain);
      last[remain] = 0x80;
      if (64 - (remain + 1) >= sizeof(uint64_t)) { // We have room at the end
        *((uint64_t*)(&last[64-sizeof(uint64_t)])) = htobe64(nbytes*8);
        curr = hashBlock(curr, (uint32_t*)last);
      } else { // Hash as is, then do last block
        curr = hashBlock(curr, (uint32_t*)last);
        memset(last, 0, sizeof(last));
        *((uint64_t*)(&last[64-sizeof(uint64_t)])) = htobe64(nbytes*8);
        curr = hashBlock(curr, (uint32_t*)last);
      }

    }
    total += 64;
  }


  return curr;
}




int main() {
  const char s[] = "hello world";
  char data[127] = {};
  SHA256Hash test = sha256(data, sizeof(data));

  for (int i=0; i < 8; i++) {
    printf("%08x\n", test.hash[i]);
  }

}
