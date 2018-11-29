VHLS_INC := $(shell ./vivado-include)
GMP_LINK_DIRECTIVE := $(shell ./gmp-link-directive)

CFLAGS = -I${VHLS_INC} -DHLS_NO_XIL_FPO_LIB ${GMP_LINK_DIRECTIVE} -Wall -Werror -Wno-unused-label -Wno-unknown-pragmas -Wno-strict-overflow -O3

ifeq ($(MODE), FPGA_SIM)
  MODE_FLAGS = -DFPGA_SIM
	TARGET_NAME = rsa-sim
else ifeq ($(MODE), FPGA_REAL)
  MODE_FLAGS = -DFPGA_REAL
	TARGET_NAME = rsa-host
else
  TARGET_NAME = rsa
endif

$(TARGET_NAME): rsa.cc fpga_rsa.cc $(wildcard *.h)
	g++ $(filter %.cc, $^) -o $@ $(CFLAGS) $(MODE_FLAGS)

clean:
	rm -rf rsa rsa-host rsa-sim
