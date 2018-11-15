//=========================================================================
// bnn_test.cpp
//=========================================================================
// @brief: testbench for Binarized Neural Betwork(BNN) digit recongnition application

#include <iostream>
#include <fstream>
#include "aes.h"
#include "timer.h"

using namespace std;

// Number of test instances
const int TEST_SIZE = 100;

//------------------------------------------------------------------------
// Helper function for reading images and labels
//------------------------------------------------------------------------

void read_test_images(int8_t test_images[TEST_SIZE][256]) {
  std::ifstream infile("data/test_b.dat");
  if (infile.is_open()) {
    for (int index = 0; index < TEST_SIZE; index++) {
      for (int pixel = 0; pixel < 256; pixel++) {
        int i;
        infile >> i;
        test_images[index][pixel] = i;
      }
    }
    infile.close();
  }
}

void read_test_labels(int test_labels[TEST_SIZE]) {
  std::ifstream infile("data/label.dat");
  if (infile.is_open()) {
    for (int index = 0; index < TEST_SIZE; index++) {
      infile >> test_labels[index];
    }
    infile.close();
  }
}

//------------------------------------------------------------------------
// Digitrec testbench
//------------------------------------------------------------------------

int main(){
  // HLS streams for communicating with the cordic block
  hls::stream<bit32_t> digitrec_in;
  hls::stream<bit32_t> digitrec_out;
  
  Timer timer("aes encr");
  timer.start();



  timer.stop();

  int8_t test_images[TEST_SIZE][256];
  int test_labels[TEST_SIZE];
  
  // read test images and labels
  read_test_images(test_images);
  read_test_labels(test_labels);
  bit32_t test_image;
  float correct = 0.0;
  
  // Timer
  Timer timer("digirec BNN");
  timer.start();
  
  // pack images to 32-bit and transmit to dut function 
  for (int test = 0; test < TEST_SIZE; test++) {
    for (int i = 0; i < I_WIDTH1 * I_WIDTH1 / BUS_WIDTH; i++) {
      for (int j = 0; j < BUS_WIDTH; j++) {
        test_image(j,j) = test_images[test][i*BUS_WIDTH+j];
      }
      digitrec_in.write(test_image);
    }
    
    // perform prediction
    dut(digitrec_in, digitrec_out);
   
    // check results
    if (digitrec_out.read() == test_labels[test]) correct += 1.0;
  }
  timer.stop();

  // Calculate accuracy
  std::cout << "Accuracy: " << correct/TEST_SIZE << std::endl;
  
  return 0;
}
