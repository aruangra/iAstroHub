#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	FILE *pt;
	char buff[17];
	int i=1;
	int timeout=60;

	sleep(1);
	
	system("echo ISIDLE: > /tmp/gtkImager");
	
	sleep(1);

	while (1) {
	
		system("tail -1 /home/pi/www/CCD_log.txt > /home/pi/www/check_idle.txt");
		pt = fopen("/home/pi/www/check_idle.txt", "r"); // Open File
		fgets (buff,17,pt);
		fclose(pt); // Close File
		
		if (buff[13]=='1') {
		    if(access("/home/pi/www/status_capture",F_OK) == 0){
				remove("/home/pi/www/status_capture");
			}
		    if(access("/home/pi/www/status_ccd",F_OK) == 0){
				remove("/home/pi/www/status_ccd");
			}
			return;
		}

		system("echo ISIDLE: > /tmp/gtkImager");
		sleep(1);
		
		i=i+1;
		if (i>=timeout) {
		    if(access("/home/pi/www/status_capture",F_OK) == 0){
				remove("/home/pi/www/status_capture");
			}
		    if(access("/home/pi/www/status_ccd",F_OK) == 0){
				remove("/home/pi/www/status_ccd");
			}
			return;
		}
		
	}

}
