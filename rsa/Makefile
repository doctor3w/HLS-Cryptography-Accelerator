VHLS_INC := $(shell ./vivado-include)
GMP_LINK_DIRECTIVE := $(shell ./gmp-link-directive)

CFLAGS = -I${VHLS_INC} -DHLS_NO_XIL_FPO_LIB ${GMP_LINK_DIRECTIVE} -Wall -Werror -Wno-unused-label -Wno-unknown-pragmas -Wno-strict-overflow -O3

rsa: rsa.cc fpga_rsa.cc $(wildcard *.h)
	g++ $(filter %.cc, $^) -o $@ $(CFLAGS)

clean:
	rm -rf rsa
