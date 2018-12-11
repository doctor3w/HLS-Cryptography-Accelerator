# HLS Crypto Accelerator: AES+RSA+SHA
This project is composed of three subprojects: AES, RSA, and SHA. Each of these
projects were build for use on the Zedboard's Xilinx Zynq-7000.

## AES
Hello my name is Drew and this is how you should run my code


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
