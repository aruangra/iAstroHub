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
	char RA[100];
	char DEC[100];
	char ra_h[5], ra_m[5], ra_s[5];
	char dec_sign[5], dec_d[5], dec_m[5], dec_s[5];
	

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

    write(fd,":GR#",4);
	usleep(500000);
	n = read(fd,RA,100); 	// Read Data From Serial Port
	RA[n] = 0;
	pt = fopen("/home/pi/www/skychart/mount_RADEC.txt", "w"); // Open File
	fprintf(pt,"%s ",RA); // Print to file
	fclose(pt); // Close File

		
    write(fd,":GD#",4);
	usleep(500000);
	n = read(fd,DEC,100); 	// Read Data From Serial Port
	DEC[n] = 0;
	pt = fopen("/home/pi/www/skychart/mount_RADEC.txt", "a"); // Open File
	fprintf(pt,"%s\n",DEC); // Print to file
	fclose(pt); // Close File
	
	close(fd); // Close Port
	
	memcpy(ra_h,&RA[0],2);
	ra_h[2] = 0;
	memcpy(ra_m,&RA[3],2);
	ra_m[2] = 0;
	memcpy(ra_s,&RA[6],2);
	ra_s[2] = 0;
	pt=fopen("/home/pi/www/skychart/mount_RA.txt","w");
	fprintf(pt,"%s\n%s\n%s\n",ra_h,ra_m,ra_s);
	fclose(pt);
	
	memcpy(dec_sign,&DEC[0],1);
	dec_sign[1] = 0;
	memcpy(dec_d,&DEC[1],2);
	dec_d[2] = 0;
	memcpy(dec_m,&DEC[4],2);
	dec_m[2] = 0;
	memcpy(dec_s,&DEC[7],2);
	dec_s[2] = 0;
	pt=fopen("/home/pi/www/skychart/mount_DEC.txt","w");
	fprintf(pt,"%s1\n%s\n%s\n%s\n",dec_sign,dec_d,dec_m,dec_s);
	fclose(pt);
	
	return 0; // End Program
}


