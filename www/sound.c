#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int delay1;

	delay1=1;

	while (1) {

		if(access("/home/pi/www/tts",F_OK) == 0){
			if(access("/home/pi/www/speak",F_OK) == 0){
				system("festival --tts /home/pi/www/speak");
				remove("/home/pi/www/speak");
			}
		}
		
		sleep(delay1);

	}

}
