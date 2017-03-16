#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	FILE *pt;
	char buff[10];
	int i=0;

	while (1) {
	
		system("tail -1 CCD_log.txt > check_hist.txt");
		pt = fopen("check_hist.txt", "r"); // Open File
		fgets (buff,10,pt);
		fclose(pt); // Close File
		if (buff[6]=='G' && buff[7]=='E') {
			sleep(1);
			system("cp /tmp/gtkImager.jpg /home/pi/www/image.jpg");
			printf("IIIII");
			return;
		}
		if (buff[6]=='E' && buff[7]=='R'){
			printf("OOOOO");
			return;
		}

		sleep(1);
		i=i+1;
		
		if (i>=4) {
			printf("OOOOO");
			return;
		}
		
	}

}
