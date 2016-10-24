#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>



#include <sstream>
#include <string>
#include <iostream>
#include "Food.h"
#include "Food.cpp"

using namespace std;
using namespace cv;

// Initialize values used throughout the program

static const char* portName = "/dev/ttyACM0";
unsigned char c = 'D';

int main(int argc, char* argv[])
{
	
	

	int serialPort;
	struct termios portOptions;
	serialPort = open(portName, O_RDWR | O_NONBLOCK);
	tcgetattr(serialPort, &portOptions);
	tcflush(serialPort, TCIOFLUSH);
	cfsetispeed(&portOptions, B115200);
	cfsetospeed(&portOptions, B115200);


  // c_cflag contains a few important things- CLOCAL and CRcEAD, to prevent
  //   this program from "owning" the port and to enable receipt of data.
  //   Also, it holds the settings for number of data bits, parity, stop bits,
  //   and hardware flow control. 
  portOptions.c_cflag |= CLOCAL;
  portOptions.c_cflag |= CREAD;
  // Set up the frame information.
  portOptions.c_cflag &= ~CSIZE; // clear frame size info
  portOptions.c_cflag |= CS8;    // 8 bit frames
  portOptions.c_cflag &= ~PARENB;// no parity
  portOptions.c_cflag &= ~CSTOPB;// one stop bit

  tcsetattr(serialPort, TCSANOW, &portOptions);
  tcflush(serialPort, TCIOFLUSH);
  write (serialPort, "100", 3);

  return 0;
}