#=============================================================================
# run_base.tcl 
#=============================================================================
# @brief: A Tcl script for synthesizing the baseline digit recongnition design.

# Project name
set hls_prj sha512cracker.prj

# Open/reset the project
open_project ${hls_prj} -reset

# Top function of the design is "dut"
set_top dut

# Add design and testbench files
add_files SHA512.cpp
add_files unix_cracker.cpp
add_files main.cpp
add_files -tb unix_cracker_test.cpp
#add_files -tb data

open_solution "solution1"
# Use Zynq device
set_part {xc7z020clg484-1}

# Target clock period is 10ns
create_clock -period 9

### You can insert your own directives here ###
# Partition the array so 64_bit accesses are fast
set_directive_array_partition SHA512Hasher::SHA512Hasher buf -type cyclic -factor 8

set_directive_unroll read64clear/LOOP
set_directive_unroll -factor 8 memcpy_u8/LOOP
set_directive_unroll -factor 8 memset_u8/LOOP
set_directive_unroll SHA512Hasher::digest/LOOP_U64
set_directive_unroll SHA512Hasher::byte_digest/LOOP_DIGEST

# We do not want update to be inlined in calc
set_directive_inline -off SHA512Hasher::update

set_directive_array_partition SHA512Hasher::hashBlock W -type complete
set_directive_pipeline SHA512Hasher::hashBlock/LOOP16
set_directive_pipeline SHA512Hasher::hashBlock/LOOP64
# Currently this causes timing violations:
#set_directive_unroll SHA512Hasher::hashBlock/LOOP64 -factor 2
set_directive_unroll SHA512Hasher::hashBlock/LOOP_SHIFT
# set_directive_array_partition SHA512Hasher::hashBlock K	-type complete

############################################

# Simulate the C++ design
csim_design -O
# Synthesize the design
csynth_design
# Co-simulate the design
 cosim_design
exit
