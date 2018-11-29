#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class Host {
 public:
  Host() : fdr(-1), fdw(-1) {}
  ~Host() { close(); }

  bool open() {
    fdr = ::open("/dev/xillybus_read_32", O_RDONLY);
    if (fdr < 0) {
      return false;
    }

    fdw = ::open("/dev/xillybus_write_32", O_WRONLY);
    if (fdw < 0) {
      ::close(fdr);
      return false;
    }

    return true;
  }

  void close() {
    if (fdr == -1) {
      return;
    }

    ::close(fdr);
    ::close(fdw);
  }

  void write(char* buf, int len) {
    int result = ::write(fdr, (void*)buf, len);
    assert(result == len);
  }
  void read(char* buf, int len) {
    int nbytes = 0;
    while (nbytes != len) {
      int result = ::read(fdr, (void*)(buf + nbytes), len - nbytes);
      assert(result != -1);
      nbytes += result;
    }
  }

 private:
  int fdr;
  int fdw;
};
