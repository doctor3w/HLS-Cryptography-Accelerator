#include "evil.h"

class Evil {
 public:
  Evil(int x) : x(x) {}

  int as_int() {
    return x;
  }

  friend Evil operator%(const Evil& a, const Evil&b) {
    // evil mod definition
    #pragma HLS inline off
    return Evil(a.x + b.x);
  }

 private:
  int x;
};

void dut(int in, int& out) {
  out = (Evil(in) % Evil(in)).as_int();
}
