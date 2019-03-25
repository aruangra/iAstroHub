#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

int resetUSBSerial () {
    system("rmmod ftdi_sio");
    system("rmmod usbserial");
    system("modprobe usbserial");
    system("modprobe ftdi_sio");
    return 0;
}

int main (){

	int n_frames;
	int exposure;
	int delay;
	int i;
	int k;

	FILE *pt;
	
	int fd, sercmd;
	
	pt=fopen("/home/pi/www/dslr_nf.txt","r");
	fscanf(pt,"%d",&n_frames);
	fclose(pt);

	if (n_frames < 1){
		n_frames=1;
	}

	pt=fopen("/home/pi/www/dslr_ex.txt","r");
	fscanf(pt,"%d",&exposure);
	fclose(pt);

	if (exposure < 1){
		exposure=1;
	}
	
	pt=fopen("/home/pi/www/dslr_de.txt","r");
	fscanf(pt,"%d",&delay);
	fclose(pt);

	if (delay < 16){
		delay=16;
	}
	
	pt = fopen("/home/pi/www/status_dslr", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	pt = fopen("/home/pi/www/photo.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File

	sleep(2);
	
	sercmd = TIOCM_RTS;
	
		i=1;
		while (i<=n_frames) {
			resetUSBSerial();
			fd = open("/dev/ttyUSB0", O_RDONLY); // Open the serial port.
			ioctl(fd, TIOCMBIC, &sercmd); // Set the RTS pin to HIGH.
			
			pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
			fprintf(pt,"Frame %d of %d started\n",i,n_frames);
			fclose(pt); // Close File
			
			pt = fopen("/home/pi/www/speak", "w");
			fprintf(pt,"Frame %d started\n",i);
			fclose(pt); 
			
			if(access("/home/pi/www/mlu",F_OK) == 0) // Mirror lock-up section
			{
				pt = fopen("/home/pi/www/status_mlu", "w"); // Open File
				fprintf(pt,"");
				fclose(pt); // Close File		
				
				ioctl(fd, TIOCMBIS, &sercmd); // Set the RTS pin.
				sleep(1);
				ioctl(fd, TIOCMBIC, &sercmd); // Reset the RTS pin.
				sleep(2);
				
				system("rm /home/pi/www/status_mlu");
			}			

			
			if (-1==ioctl(fd, TIOCMBIS, &sercmd)) // Set the RTS pin to LOW (start exposure).
			{	
				int e = errno;
				pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
                		fprintf(pt,"Error ioctl failed %sn",strerror(e));
                		fclose(pt); // Close File; 
                		close(fd);
                		resetUSBSerial();
                		fd = open("/dev/ttyUSB0", O_RDONLY); // Open the serial port.
                		ioctl(fd, TIOCMBIS, &sercmd); // Set the RTS pin to LOW (start exposure).
            		}

			for(k=1;k<=exposure;k++)
			{
				sleep(1);
                        	pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
                        	fprintf(pt,"Frame %d of %d : Exposing %d / %d s\n",i,n_frames,k,exposure);
                        	fclose(pt); // Close File
			}

			if (-1==ioctl(fd, TIOCMBIC, &sercmd)) // Set the RTS pin to HIGH (end exposure).
			{	
				int e = errno;
				pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
                		fprintf(pt,"Error ioctl failed %sn",strerror(e));
                		fclose(pt); // Close File; 
                		close(fd);
                		resetUSBSerial();
                		fd = open("/dev/ttyUSB0", O_RDONLY); // Open the serial port.
                		ioctl(fd, TIOCMBIC, &sercmd); // Set the RTS pin to HIGH (end exposure).
            		}

			pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
			fprintf(pt,"Frame %d of %d completed\n",i,n_frames);
			fclose(pt); // Close File
			
			pt = fopen("/home/pi/www/speak", "w");
			fprintf(pt,"Frame %d completed\n",i);
			fclose(pt);
			
			sleep(delay-9);
			
			if(access("/home/pi/www/download",F_OK) == 0)
			{
				system("sudo bash /home/pi/www/dslrds_getphoto.sh &");
			}
			
			sleep(7);
			
			while(access("/home/pi/www/status_loading",F_OK) == 0) 
			{
				sleep(1);
				printf("Waiting\n");
			}
			
			sleep(2);
			i=i+1;
			
			if(access("/home/pi/www/dithering",F_OK) == 0 && i<=n_frames)
			{
				pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
				fprintf(pt,"Dithering... Next frame will start soon.\n");
				fclose(pt); // Close File
				system("sudo /home/pi/www/./remote_client 4");
			}			
			close(fd);
		}

	pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
	fprintf(pt,"Done\n");
	fclose(pt); // Close File
	
	pt = fopen("/home/pi/www/speak", "w");
	fprintf(pt,"Job completed\n");
	fclose(pt);
	
	system("rm /home/pi/www/status_dslr");
	
	return 0;

}

