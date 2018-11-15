VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS = -I${VHLS_INC} -DHLS_NO_XIL_FPO_LIB -Wall -Werror -O3

evil: $(wildcard *.cc) $(wildcard *.h)
	g++ $(filter %.cc, $^) -o $@ $(CFLAGS)

clean:
	rm -rf evil
