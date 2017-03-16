#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	FILE *pt;
	char buff[10];
	int i=0;

	while (1) {
	
		system("tail -1 /home/pi/www/CCD_log.txt > /home/pi/www/check_hist.txt");
		pt = fopen("/home/pi/www/check_hist.txt", "r"); // Open File
		fgets (buff,10,pt);
		fclose(pt); // Close File
		if (buff[6]=='P' && buff[7]=='R') {
			return;
		}

		sleep(1);
		i=i+1;
		
		if (i>=4) {
			return;
		}
		
	}

}
