#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int k;
	FILE *pt;
	char buff[10];
	
	pt = fopen("/home/pi/www/status_ccd", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	pt = fopen("image.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File


			pt = fopen("image.txt", "a"); // Open File
			fprintf(pt,"Capturing...\n");
			fclose(pt); // Close File
			
			system("echo RUN: > /tmp/gtkImager");
			
			pt = fopen("/home/pi/www/status_capture", "w"); // Open File
			fprintf(pt,"");
			fclose(pt); // Close File
			
			sleep(1);
			k=1;
		
			while(access("/home/pi/www/status_capture",F_OK) == 0) 
			{

				system("tail -1 /home/pi/www/CCD_log.txt > /home/pi/www/check_status_capture.txt");
				pt = fopen("/home/pi/www/check_status_capture.txt", "r"); // Open File
				fgets (buff,10,pt);
			    fclose(pt); // Close File
				if ( (buff[6]=='P' && buff[7]=='R') || (buff[6]=='C' && buff[7]=='A') ){
					system("rm /home/pi/www/status_capture");
				}
				
				// Detect error
				if ( (buff[6]=='E' && buff[7]=='R') ){
					pt = fopen("image.txt", "a"); // Open File
					fprintf(pt,"Error!\n");
					fclose(pt); // Close File					
					sleep(2);
					system("rm /home/pi/www/status_capture");
					system("rm /home/pi/www/status_ccd");
					return(0);
				}
				
				sleep(1);
				k=k+1;				

			}
			
			pt = fopen("image.txt", "a"); // Open File
			fprintf(pt,"Saving...\n");
			fclose(pt); // Close File

			sleep(2);
						
			system("cp /home/pi/www/tmp_images/*.jpg /home/pi/www/autofocus/tmp_images/preview.jpg");
			system("cp /home/pi/www/tmp_images/*.txt /home/pi/www/histogram.txt");
			system("rm /home/pi/www/tmp_images/*.*");
			system("perl /home/pi/www/histogram_split.pl");
			
	pt = fopen("image.txt", "a"); // Open File
	fprintf(pt,"Select a star.\n");
	fclose(pt); // Close File
	sleep(2);
	
	system("rm /home/pi/www/status_ccd");
	
}

