src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS = -I${VHLS_INC} -DHLS_NO_XIL_FPO_LIB -std=c++0x -Wall

sha512: $(src)
	$(CXX) -o $@ $^ $(CFLAGS)

.PHONY: clean
	rm -rf sha512

