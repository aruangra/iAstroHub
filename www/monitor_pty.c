#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int delay1;

	delay1=200000;

	while (1) {

		if(access("/home/pi/www/start_socat",F_OK) == 0){
			system("perl /home/pi/www/start_socat.pl");
			remove("/home/pi/www/start_socat");
		}

         if(access("/home/pi/www/stop_socat",F_OK) == 0){
            system("sh /home/pi/www/stop_socat.sh");
            remove("/home/pi/www/stop_socat");
        }

		usleep(delay1);

	}

}
