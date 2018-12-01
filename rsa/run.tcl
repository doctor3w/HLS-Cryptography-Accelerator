set CFLAGS "-DHLS_ENABLE_PRAGMAS=1 -DFPGA_SIM"
set hls_prj "rsa.prj"
open_project ${hls_prj} -reset
set_top dut

add_files fpga_rsa.cc -cflags $CFLAGS
add_files -tb rsa.cc -cflags $CFLAGS

open_solution "solution1"
set_part {xc7z020clg484-1}
create_clock -period 8.5

csim_design
csynth_design
# cosim_design

quit
