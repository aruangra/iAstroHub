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
	
	double ra, dec;
	char cmd[500];
	
	pt=fopen("/home/pi/www/start_socat","w");
	fclose(pt);	
	sleep(1);

	/* Open Port */
	fd = open("/dev/netcom0", O_RDWR | O_NOCTTY | O_NDELAY);

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


	pt=fopen("/home/pi/www/skychart2TSX/coord_RA.txt","r");
	fscanf(pt,"%d\n%d\n%d",&ra_h,&ra_m,&ra_s);
	fclose(pt);
	ra=(ra_s/60.+ra_m)/60.+ra_h;

	pt=fopen("/home/pi/www/skychart2TSX/coord_DEC.txt","r");
	fscanf(pt,"%d\n%d\n%d\n%d",&dec_sign,&dec_d,&dec_m,&dec_s);
	fclose(pt);
	dec=((dec_s/60.+dec_m)/60.+dec_d)*dec_sign;
	
	sprintf(cmd,"/* Java Script */var TargetRA = \"%.6f\";var TargetDec = \"%.6f\";var Out;sky6RASCOMTele.Connect();if (sky6RASCOMTele.IsConnected==0) {Out = \"Not connected\"} else {sky6RASCOMTele.Sync(TargetRA, TargetDec,\"\");Out = \"OK\";}//_",ra,dec);
	
    write(fd,cmd,string_length(cmd));
    usleep(500000);
	
	close(fd); // Close Port
	
	pt=fopen("/home/pi/www/stop_socat","w");
	fclose(pt);
	sleep(1);
	
	return 0; // End Program
}

int string_length(char str[])
{
   int i;
   for(i=0; i<500; i++)
   {
	 if(str[i]=='_')
	 {
	    return(i+1);
	 }
   }
}


