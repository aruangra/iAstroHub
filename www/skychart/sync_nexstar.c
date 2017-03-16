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

	char RADEC[100];
	unsigned ra, dec;
	int i;

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


	/* Read RA&DEC from file */
	pt=fopen("/home/pi/www/skychart/coord_RA.txt","r");
	fscanf(pt,"%d\n%d\n%d",&ra_h,&ra_m,&ra_s);
	fclose(pt);

	pt=fopen("/home/pi/www/skychart/coord_DEC.txt","r");
	fscanf(pt,"%d\n%d\n%d\n%d",&dec_sign,&dec_d,&dec_m,&dec_s);
	fclose(pt);

	/* Populate command string in hexadecimal */
	ra = (ra_s*15+ra_m*15*60+ra_h*3600*15)*4294967296/360/60/60;
	dec = (dec_s+dec_m*60+dec_d*3600)*4294967296/360/60/60;
	if (dec_sign == -1)
	{
		dec = 4294967296 - dec;
	}
	sprintf(RADEC,"s%8X,%8X",ra,dec);
	for (i=0;i<=17;i++)	// Replace space with 0
	{
		if ( RADEC[i] == 32 )
			RADEC[i] = '0';
	}

	/* Set RADEC */
    	sleep(1);

	write(fd,RADEC,18); //Sync
	usleep(500000);

	close(fd); // Close Port
	return 0; // End Program
}

int string_length(char str[])
{
   int i;
   for(i=0; i<80; i++)
   {
	 if(str[i]=='#')
	 {
	    return(i+1);
	 }
   }
}

