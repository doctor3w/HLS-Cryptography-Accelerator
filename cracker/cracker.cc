/* These are all the valid characters that could be in a password */
#include <gmp.h>
#include <stdio.h>

#include "sha256.h"

#define NUM_PRINTABLE_CHARS 95
extern const char printable_chars[NUM_PRINTABLE_CHARS] = 
  {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 45, 56, 47, 48, 49, 50,
   51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
   69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 
   87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 
   104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 
   118, 119, 120, 121, 122, 123, 124, 125, 126};

#define NUM_ALPHA_CHARS 52
extern const char alpha_chars[NUM_ALPHA_CHARS] =
   {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 
    83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99, 100, 101, 102, 103, 104, 
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 
    119, 120, 121, 122};

bool operator==(const SHA256Hash& h1, const SHA256Hash& h2) {
  return h1.hash[0] == h2.hash[0] && h1.hash[1] == h2.hash[1] && 
         h1.hash[2] == h2.hash[2] && h1.hash[3] == h2.hash[3] &&
         h1.hash[4] == h2.hash[4] && h1.hash[5] == h2.hash[5] && 
         h1.hash[6] == h2.hash[6] && h1.hash[7] == h2.hash[7];
} 

template<int N, const char valid_chars[], int num_valid_chars> struct permute;

template<const char valid_chars[], int num_valid_chars> 
struct permute<4, valid_chars, num_valid_chars> {
  void operator() (SHA256Hash hash_to_crack) {
    char data[127] = {};
    SHA256Hash hash;
    for (int i1 = 0; i1 < num_valid_chars; i1++)
      for (int i2 = 0; i2 < num_valid_chars; i2++)
        for (int i3 = 0; i3 < num_valid_chars; i3++)
          for (int i4 = 0; i4 < num_valid_chars; i4++) {
            data[0] = valid_chars[i1];
            data[1] = valid_chars[i2];
            data[2] = valid_chars[i3];
            data[3] = valid_chars[i4];
            hash = sha256(data, sizeof(data));
            if (hash == hash_to_crack) {
              printf("Cracked! The text is %s\n", data);
              return;
            }
          }
  }
};

template<const char valid_chars[], int num_valid_chars> 
struct permute<8, valid_chars, num_valid_chars> {
  void operator() (SHA256Hash hash_to_crack) {
    char data[127] = {};
    SHA256Hash hash;
    for (int i1 = 0; i1 < num_valid_chars; i1++)
      for (int i2 = 0; i2 < num_valid_chars; i2++)
        for (int i3 = 0; i3 < num_valid_chars; i3++)
          for (int i4 = 0; i4 < num_valid_chars; i4++)
            for (int i5 = 0; i5 < num_valid_chars; i5++)
              for (int i6 = 0; i6 < num_valid_chars; i6++)
                for (int i7 = 0; i7 < num_valid_chars; i7++)
                  for (int i8 = 0; i8 < num_valid_chars; i8++) {
                    data[0] = valid_chars[i1];
                    data[1] = valid_chars[i2];
                    data[2] = valid_chars[i3];
                    data[3] = valid_chars[i4];
                    data[4] = valid_chars[i5];
                    data[5] = valid_chars[i5];
                    data[6] = valid_chars[i6];
                    data[7] = valid_chars[i7];
                    hash = sha256(data, sizeof(data));
                    if (hash == hash_to_crack) {
                      printf("Cracked! The text is %s\n", data);
                      return;
                    }
          }
  }
};

int main() {
  char data[127] = {};
  data[0] = 'H';
  data[1] = 'E';
  data[2] = 'L';
  data[3] = 'P';
  SHA256Hash test = sha256(data, sizeof(data));
  
  permute<4, alpha_chars, NUM_ALPHA_CHARS> p;
  p(test);

  printf ("\nThe hash is:\n");
  for (int i=0; i < 8; i++) {
    printf("%08x\n", test.hash[i]);
  }
}
