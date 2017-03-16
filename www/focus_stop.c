#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

int main()
{

	int fd;
	struct termios options;

	char rf_cmd[110];

	/* Open Port */
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

	if(fd == -1) {
		printf("ERROR Open Serial Port!");
	}

	/* Serial Configuration */
	tcgetattr(fd, &options); 	// Get Current Config
	cfsetispeed(&options, B9600); // Set Baud Rate
	cfsetospeed(&options, B9600);
	options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;
	options.c_iflag =  IGNBRK;
	options.c_lflag = 0;
	options.c_oflag = 0;
	options.c_cflag |= CLOCAL | CREAD;
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 5;
	options.c_iflag &= ~(IXON|IXOFF|IXANY);
	options.c_cflag &= ~(PARENB | PARODD);
	/* Save The Configure */
	tcsetattr(fd, TCSANOW, &options);
	/* Flush the input (read) buffer */
	tcflush(fd,TCIOFLUSH);
	
	sprintf(rf_cmd, "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
	
    write(fd,rf_cmd,100);
		
	close(fd); // Close Port
	return 0; // End Program
}

int CalculateSum(char *rf_cmd)
{

  int val= 0 ;
  int i=0 ;

  for(i=0; i < 8; i++) {

    val = val + (unsigned char) rf_cmd[i] ;
  }

  return val % 256 ;
}


