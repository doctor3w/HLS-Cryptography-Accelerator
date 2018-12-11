# HLS Crypto Accelerator: AES+RSA+SHA
This project is composed of three subprojects: AES, RSA, and SHA. Each of these
projects were build for use on the Zedboard's Xilinx Zynq-7000.

## AES
The entire AES implementation is in the `aes` directory:
- `aes.cpp`: The optimized synthesizable AES CTR implementation. 
- `aes.h`: The header containing some definitions for the AES implementation including 128, 192, or 256 key sizes as well as the number of blocks that will be encrypted with the FPGA. 
- `aes_test.cpp`: The AES test file for csim. 
- `aes-sw.c/aes-sw.h/test-sw.c`: The embedded software optimized version from tiny-AES-c. 
- `data`: Directory containing the random bytes and the encrypted solutions. 

The `Makefile` contains all the commands necessary to build the software. 

- `make`: Builds and runs the `aes.cpp` file. 
- `make sw`: Builds and runs the optimized AES program. 
- `make fpga`: Builds and runs the host file for communicating with the FPGA once the bitstream has been loaded. 
- `make bitsream`: Runs the bitstream making script. 

Running `vivado_hls -f run.tcl` will synthesize the software. 

## RSA
The entire RSA implementation is in the `rsa` directory:
- `big_ap_int.h`: header used to include `ap_int` safely.
- `bignum.h`: header containing large number arithmetic library.
- `fpga_rsa.h`, `fpga_rsa.cc`: top-level design under test
- `fpga_timer.h`: wall-clock based timer for FPGA performance analysis.
- `sim_timer.h`: CPU time based timer for software performance analysis.
- `host.h`: small library for xillybus communication.
- `mpz_adapters.h`: algorithms to convert between `Bignum` and `mpz_t` types.
- `pragmas.h`: macros for controlling HLS pragmas
- `rsa.cc`: main program, which encrypts a small message
- `rsa_config.h`: contains macro definitions which define RSA key sizes and such.

There are some additional files as well:
- `data`: a directory containing final benchmarking data.
- `format.sh`: a formatting script.
- `bench`: a benchmarking script.
- `gmp-link-directive` and `vivado-include` two helper scripts which allow using the same makefile on both the ZedBoard and x86 machines.
- `run_bitstream.sh`: modified bitstream script which takes a project name as a parameter.
- `run.tcl`: main tcl file.

The `Makefile` contains all the command necessary to build the software:
- `make`: builds optimized software version (`rsa`).
- `make MODE=FPGA_SIM`: builds a software version which runs the synthesizable code (`rsa-sim`).
- `make MODE=FPGA_REAL`: builds a software version which communicates with the FPGA (`rsa-host`).

Running `vivado_hls -f run.tcl` will generate `rsa.prj`. Running `./run_bitstream rsa` will generate the bitstream, `rsa.bit`.

## SHA
The entire HLS implementation can be found symlinked in the `ecelinux` directory:
- `SHA512.cpp/SHA512.h`: Contains the SHA-512 implementation wrapped in the `SHA512Hasher` class
- `unix_cracker.cpp/unix_cracker.h`: Contains the Unix ``crypt()`` implementation
- `main.cpp`: The HLS entry point (`dut()`)
- `helpers.cpp/helpers.h`: Several basic helper functions
- `unix_cracker_test.cpp`: The csim test program


The Zedboard's host program can be found in the `zedboard` directory.


### Building
To generate the bitstream, run "vivado_hls run.tcl" in the ecelinux directory.
Note you must be using the 2017 version.
The bitstream can be generated afterwards with the ./run_bitstream.sh script.

To generate the Zedboard's host program, run make in the Zedboard directory.


### Running
Flash the bitstream to a Zedboard, then excute the host program that was compiled
in the previous step.
