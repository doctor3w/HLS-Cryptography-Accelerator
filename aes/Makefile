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

CFLAGS = -g -I${VHLS_INC} 

all: csim

csim: aes.cpp aes_test.cpp
	@echo "Compiling & simulating digitrec on amdpool ..."
	g++ ${CFLAGS} $^ -o aes -lrt
	./aes

bitstream: 
	@echo "Synthesizing digitrec and creating bitstream on amdpool ..."
	vivado_hls -f run_pipeline.tcl
	./run_bitstream.sh

clean:
	rm -rf aes *.dat *.prj *.log
	rm -rf zedboard_project* xillydemo.bit
