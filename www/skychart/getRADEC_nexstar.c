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
	char RADEC[50];
	unsigned ra, dec;
	float f_ra, f_dec;
	int ra_h, ra_m, ra_s;
	int dec_sign, dec_d, dec_m, dec_s;
	

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

    /* Get RA&DEC in hexadecimal,and convert to unsigned int. */
    write(fd,"e",1);
	usleep(500000);
	n = read(fd,RADEC,50); 	// Read Data From Serial Port
	RADEC[n] = 0;
	sscanf(RADEC,"%X,%X#",&ra,&dec);
	//pt = fopen("/home/pi/www/skychart/mount_RADEC.txt", "w"); // Open File

    close(fd); // Close Port

	/* Convert RA to H:M:S */
	f_ra = (float)ra;
	f_ra = f_ra*360*60*60/4294967296;
	ra_h = f_ra/3600/15;
	ra_m = (f_ra-ra_h*15*3600)/60/15;
	ra_s = (f_ra-ra_h*15*3600-ra_m*60*15)/15;
	pt=fopen("/home/pi/www/skychart/mount_RA.txt","w");
	fprintf(pt,"%d\n%d\n%d\n",ra_h,ra_m,ra_s);
	fclose(pt);
	
	/* Convert DEC to <s>:D:M:S */
	if (dec>270*4294967296/360)
	{
		dec = 4294967296 - dec;
		dec_sign = -1;
	} else dec_sign = 1;

	f_dec = (float)dec;
	f_dec = f_dec*360*60*60/4294967296;
	dec_d = f_dec/3600;
	dec_m = (f_dec-dec_d*3600)/60;
	dec_s = (f_dec-dec_d*3600-dec_m*60);
	pt=fopen("/home/pi/www/skychart/mount_DEC.txt","w");
	fprintf(pt,"%d\n%d\n%d\n%d\n",dec_sign,dec_d,dec_m,dec_s);
	fclose(pt);
	
	return 0; // End Program
}

