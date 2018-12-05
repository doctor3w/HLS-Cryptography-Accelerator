#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and synthesizes the digitrec program
#
# @desc: 1. "make" runs csim by default
#        2. "make csim" compiles & executes the fixed-point implementation
#        3. "make clean" cleans up the directory


# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include
INC_PATH=/usr/include/vivado_hls/2015.2

CFLAGS = -g -I${VHLS_INC} 
FCFLAGS = -g -I${INC_PATH} -lrt

all: csim

csim: aes.cpp aes_test.cpp
	@echo "Compiling & simulating aes on amdpool ..."
	g++ ${CFLAGS} $^ -o aes -lrt
	./aes

sw: aes-sw.c test-sw.c
	@echo "Compiling & executing aes software program on ARM ..."
	g++ ${CFLAGS} $^ -o aes-arm -lrt
	./aes-arm

fpga: host.cpp
	@echo "Compiling host program & running aes accelerator ..."
	g++ ${FCFLAGS} $^ -o aes-fpga
	./aes-fpga

bitstream: 
	@echo "Synthesizing aes and creating bitstream on amdpool ..."
	vivado_hls -f run.tcl
	./run_bitstream.sh

clean-sw:
	rm -rf aes-arm

clean:
	rm -rf aes-fpga
	rm -rf aes *.dat *.prj *.log
	rm -rf zedboard_project* xillydemo.bit
