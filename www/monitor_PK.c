#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int delay1;

	delay1=1;

	while (1) {

		if(access("/home/pi/www/startPK",F_OK) == 0){
			system("sudo sh /home/pi/www/startPK.sh &");
			remove("/home/pi/www/startPK");
		}
		sleep(delay1);

	}

}
