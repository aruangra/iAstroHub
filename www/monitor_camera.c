#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int delay1;

	delay1=1;

	while (1) {

		if(access("/home/pi/www/startdslr",F_OK) == 0){
			system("sudo /home/pi/www/./dslr &");
			remove("/home/pi/www/startdslr");
		}
		sleep(delay1);

	}

}
