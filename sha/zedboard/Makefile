INC_PATH=/usr/include/vivado_hls/2015.2
CFLAGS = -O3 -I${INC_PATH} -std=c++11 -g

all: fpga

fpga: host.cpp 
	@echo "Compiling host program & running sha512 accelerator ..."
	g++ ${CFLAGS} $^ -o sha512-fpga -lrt
	./sha512-fpga

crypt_test: crypt_test.cpp
	@echo "Benchmarking with the crypt() function ..."
	g++ ${CFLAGS} $^ -o crypt-test -lrt -lcrypt
	./crypt-test

# Clean up the directory before submission
clean:
	@echo "Clean up output files"
	rm -rf sha512-arm vivado_hls.log *.prj result out.dat *~
	rm -rf sha512-fpga
