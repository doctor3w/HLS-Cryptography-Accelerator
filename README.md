# HLS Crypto Accelerator: AES+RSA+SHA
This project is composed of three subprojects: AES, RSA, and SHA. Each of these
projects were build for use on the Zedboard's Xilinx Zynq-7000.

## AES
The entire AES implementation is in the `aes` directory:
- __aes.cpp__: The optimized synthesizeable AES CTR implementation. 
- __aes.h__: The header containing some definitions for the AES implementation including 128, 192, or 256 key sizes as well as the number of blocks that will be encrypted with the FPGA. 
- __aes_test.cpp__: The AES test file for csim. 
- __aes-sw.c/aes-sw.h/test-sw.c__: The embedded software optimized version from tiny-AES-c. 
- __data__: Directory containing the random bytes and the encrypted solutions. 

The `Makefile` contains all the commands necessary to build the software. 

- `make`: Builds and runs the `aes.cpp` file. 
- `make sw`: Builds and runs the optimized AES program. 
- `make fpga`: Builds and runs the host file for communicating with the FPGA once the bitstream has been loaded. 
- `make bitsream`: Runs the bitstream making script. 

Running `vivado_hls -f run.tcl` will synthesize the software. 

## RSA
Hello my name is Jacob and this is how you should run my code


## SHA
The entire HLS implementation can be found symlinked in the `ecelinux` directory:
- __SHA512.cpp/SHA512.h__: Contains the SHA-512 implementation wrapped in the __SHA512Hasher__ class
- __unix_cracker.cpp/unix_cracker.h__: Contains the Unix ``crypt()`` implementation
- __main.cpp__: The HLS entry point (`dut()`)
- __helpers.cpp/helpers.h__: Several basic helper functions
- __unix_cracker_test.cpp__: The csim test program


The Zedboard's host program can be found in the `zedboard` directory.


### Building
To generate the bitstream, run "vivado_hls run.tcl" in the ecelinux directory.
Note you must be using the 2017 version.
The bitstream can be generated afterwards with the ./run_bitstream.sh script.

To generate the Zedboard's host program, run make in the Zedboard directory.


### Running
Flash the bitstream to a Zedboard, then excute the host program that was compiled
in the previous step.
