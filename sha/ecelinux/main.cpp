//==========================================================================
// bnn.cpp
//==========================================================================
// @brief: A convolution kernel for CNN on digit recognition

#include "unix_cracker.h"
#include "typedefs.h"

#include <hls_stream.h>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(
    hls::stream<uint8_t> &strm_in,
    hls::stream<uint8_t> &strm_out
)
{	
	char passwd[16];
	for (int i=0; i < 6; i++) {
		passwd[i] = strm_in.read();
	}

     	char hash[HASH_LEN];
	//const char passwd[] = "This is my password!";
	const char salt[] = "8n./Hzqd";
	calc(hash, passwd, sizeof(passwd)-1, salt, sizeof(salt)-1);
	

	for (int i=0; i < 86; i++) {
		strm_out.write(hash[i]);        
        }

}
