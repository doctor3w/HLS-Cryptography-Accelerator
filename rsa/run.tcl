set CFLAGS ""
set hls_prj "rsa.prj"
# Comment me out and then loops have undefined trip counts...
# set CFLAGS "-DUSE_LABEL"

open_project ${hls_prj} -reset
set_top fpga_powm

add_files fpga_rsa.cc -cflags $CFLAGS
add_files -tb rsa.cc -cflags $CFLAGS

open_solution "solution1"
set_part {xc7z020clg484-1}
create_clock -period 10

csim_design
csynth_design

# We will skip C-RTL cosimulation for now
#cosim_design

quit
