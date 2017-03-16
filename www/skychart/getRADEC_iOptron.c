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
	int n;
	char RADEC[20];
	char RA[9], DEC[9];
	int ra, ra_h, ra_m, ra_s;
	int dec, dec_sign, dec_d, dec_m, dec_s;
	

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

    
	/* Get RA and DEC from the mount*/
  	write(fd,":GEC#",5);
	usleep(500000);
	n = read(fd,RADEC,20);
	RADEC[n] = 0;
	/* Get DEC*/
	if (RADEC[0]=='+') dec_sign = 1;
	if (RADEC[0]=='-') dec_sign = -1;
	memcpy(DEC,&RADEC[1],8);
	DEC[8] = 0;
	dec = atoi(DEC);
	dec_d = dec/360000;
	dec_m = (dec-dec_d*360000)/6000;
	dec_s = (dec-dec_d*360000-dec_m*6000)/100;
	/* Get RA*/
	memcpy(RA,&RADEC[9],8);
	RA[8] = 0;
	ra = atoi(RA);
	ra_h = ra/3600000;
	ra_m = (ra-ra_h*3600000)/60000;
	ra_s = (ra-ra_h*3600000-ra_m*60000)/1000;

	pt = fopen("/home/pi/www/skychart/mount_RADEC.txt", "w"); // Open File
	fprintf(pt,"%dH:%dM:%dS ",ra_h,ra_m,ra_s); //Print to file
	fprintf(pt,"%c%dD:%dM:%dS\n",RADEC[0],dec_d,dec_m,dec_s); //Print to file
	fclose(pt); // Close File
	
	close(fd); // Close Port
	
	pt=fopen("/home/pi/www/skychart/mount_RA.txt","w");
	fprintf(pt,"%d\n%d\n%d\n",ra_h,ra_m,ra_s);
	fclose(pt);
	
	pt=fopen("/home/pi/www/skychart/mount_DEC.txt","w");
	fprintf(pt,"%d\n%d\n%d\n%d\n",dec_sign,dec_d,dec_m,dec_s);
	fclose(pt);
	
	return 0; // End Program
}
