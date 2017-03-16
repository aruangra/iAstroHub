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

	FILE *pt;
	int ra_h, ra_m, ra_s;
	int dec_sign, dec_d, dec_m, dec_s;

	char sign;
	unsigned char buffer[16];
	
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


	pt=fopen("/home/pi/www/AM_RA_JNow.txt","r");
	fscanf(pt,"%d\n%d\n%d",&ra_h,&ra_m,&ra_s);
	fclose(pt);

	pt=fopen("/home/pi/www/AM_DEC_JNow.txt","r");
	fscanf(pt,"%d\n%d\n%d\n%d",&dec_sign,&dec_d,&dec_m,&dec_s);
	fclose(pt);
	
	if (dec_sign > 0){
		sign='+';
	} else {
		sign='-';
	}	
	sprintf(buffer,"D%.2d%.2d%.2d%c%.2d%.2d%.1d",ra_h,ra_m,(int)(ra_s/0.6),sign,dec_d,dec_m,(int)(dec_s/6.0));
	buffer[13]=0;
		
	write(fd,buffer,strlen(buffer));
	write(fd, "\r\n", 2);
	sleep(1);
	
	close(fd); // Close Port
	return 0; // End Program
}
