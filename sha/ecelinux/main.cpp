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
    hls::stream<ap_uint<32> > &strm_in,
    hls::stream<ap_uint<32> > &strm_out
)
{	
	char passwd[MAX_PWD_LEN];
        int i;
	for (i=0; i < sizeof(passwd); i++) {
	  passwd[i] = strm_in.read();
          if (passwd[i] == 0) break;
	}

     	char hash[HASH_LEN];
	//const char passwd[] = "This is my password!";
	const char salt[] = "8n./Hzqd";
	calc(hash, passwd, i, salt, sizeof(salt)-1);
	

	for (int i=0; i < HASH_LEN; i++) {
		strm_out.write(hash[i]);        
        }

}
