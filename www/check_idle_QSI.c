#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	FILE *pt;
	char buff[50];
	int i=1;
	int timeout=60;
		
	while (1) {	
	
		system("sudo tail -1 /root/GoQat/log.txt > check_status_capture.txt");
		pt = fopen("check_status_capture.txt", "r"); // Open File
		fgets (buff,50,pt);
		fclose(pt); // Close File
		
		if ( (buff[38]=='F' && buff[39]=='I') ){
		    if(access("/home/pi/www/status_capture",F_OK) == 0){
				remove("/home/pi/www/status_capture");
			}
		    if(access("/home/pi/www/status_ccd",F_OK) == 0){
				remove("/home/pi/www/status_ccd");
			}
			if(access("/home/pi/www/focuserlink",F_OK) == 0){
				system("echo 'open_focuser_comms 0' > /home/pi/www/Commands.txt");
			}
			return;
		}
		
		sleep(1);
		i=i+1;
		if (i>=timeout) {
		    if(access("/home/pi/www/status_capture",F_OK) == 0){
				remove("/home/pi/www/status_capture");
			}
		    if(access("/home/pi/www/status_ccd",F_OK) == 0){
				remove("/home/pi/www/status_ccd");
			}
			if(access("/home/pi/www/focuserlink",F_OK) == 0){
				system("echo 'open_focuser_comms 0' > /home/pi/www/Commands.txt");
			}
			return;
		}
		
	}

}
