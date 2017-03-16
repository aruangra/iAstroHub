#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int size,j,k,delay=3;
	unsigned int point,dir,pos_x,pos_y;
	float fwhm;
	FILE *pt;
	char buff[10];
	char step[10];
	char cmd1[300],cmd2[300];
		
	pt = fopen("vcurve.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	pt = fopen("vcurve_x.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	pt = fopen("vcurve_y.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	pt = fopen("imagefile.txt", "w"); // Open File
	fprintf(pt,"<select name=\"imagefile\" id=\"imagefile\" style=\"background-color:#000000; color:#FF0000\">\n");
	fprintf(pt,"</select>\n");
	fclose(pt); // Close File

	pt=fopen("vcurve_point.txt","r");
	fscanf(pt,"%d",&point);
	fclose(pt);	
	
	pt=fopen("vcurve_dir.txt","r");
	fscanf(pt,"%d",&dir);
	fclose(pt);
	
	pt=fopen("vcurve_size.txt","r");
	fscanf(pt,"%d",&size);
	fclose(pt);
	
	system("sudo /home/pi/www/./focus_update");
	
	pt=fopen("/home/pi/www/current_pos.txt","r");
	fgets(step,10,pt);
	fclose(pt);
	
	pt = fopen("vcurve.txt", "a"); // Open File
	fprintf(pt,"+++++%s\n",step);
	fclose(pt); // Close File
	
	sleep(2);
	
	j=1;
	
	while (j<=point) {
	
			pt = fopen("/home/pi/www/status_ccd", "w"); // Open File
			fprintf(pt,"");
			fclose(pt); // Close File
			
			
			system("echo GETROIPOS: > /tmp/gtkImager");
			sleep(1);
			system("tail -1 /home/pi/www/CCD_log.txt > /home/pi/www/check_status_capture.txt");
			pt = fopen("/home/pi/www/check_status_capture.txt", "r"); // Open File
			fgets (buff,10,pt);
			fclose(pt); // Close File
			if ( (buff[6]=='E' && buff[7]=='R') ){
				system("rm /home/pi/www/status_ccd");
				pt = fopen("vcurve.txt", "a"); // Open File
				fprintf(pt,"^^^^^^^^^^\n");
				fclose(pt); // Close File	
				return 0;
			}
			
			pt = fopen("vcurve.txt", "a"); // Open File
			fprintf(pt,">>>>>Capturing...\n");
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
				
				if ( (buff[6]=='E' && buff[7]=='R') ){
					system("rm /home/pi/www/status_capture");
					system("rm /home/pi/www/status_ccd");
					pt = fopen("vcurve.txt", "a"); // Open File
					fprintf(pt,"eeeeeeeeee\n");
					fclose(pt); // Close File	
					return 0;
				}			

				sleep(1);
				k=k+1;

			}
			
			system("echo GETROIPOS: > /tmp/gtkImager");
			sleep(1);
			pt=fopen("/tmp/roipos.txt","r");
			fscanf(pt,"%d\n",&pos_x);
			fscanf(pt,"%d\n",&pos_y);
			fclose(pt);
			
			system("echo GETFWHM: > /tmp/gtkImager");
			sleep(1);
			pt=fopen("/tmp/fwhm.txt","r");
			fscanf(pt,"%f",&fwhm);
			fclose(pt);
			
			system("rm /home/pi/www/status_ccd");
			
			system("cp /home/pi/www/tmp_images/*.jpg /home/pi/www/autofocus/tmp_images/tmp_image.jpg");
			system("rm /home/pi/www/tmp_images/*.*");
			sprintf(cmd1,"sudo convert /home/pi/www/autofocus/tmp_images/tmp_image.jpg -crop %dx%d+%d+%d /home/pi/www/autofocus/tmp_images/image.jpg",size,size,pos_x-size/2,pos_y-size/2);
			system(cmd1);
			sprintf(cmd2,"sudo cp /home/pi/www/autofocus/tmp_images/image.jpg /home/pi/www/autofocus/images/%s.jpg",step);
			system(cmd2);
			
			pt = fopen("vcurve_x.txt", "a"); // Open File
			fprintf(pt,"%s ",step);
			fclose(pt); // Close File
			
			pt = fopen("vcurve_y.txt", "a"); // Open File
			fprintf(pt,"%f ",fwhm);
			fclose(pt); // Close File
			
			system("sed -ie '$d' imagefile.txt");
			pt = fopen("imagefile.txt", "a"); // Open File
			fprintf(pt,"<option value=\"%s.jpg\">%s</option>\n",step);
			fprintf(pt,"</select>\n");
			fclose(pt); // Close File
			
			pt = fopen("vcurve.txt", "a"); // Open File
			fprintf(pt,"-----%f\n",fwhm);
			fclose(pt); // Close File
			
			sleep(2);
			
			j=j+1;
			
			if (j<=point) {
			
			
				pt = fopen("vcurve.txt", "a"); // Open File
				fprintf(pt,"=====Moving...\n");
				fclose(pt); // Close File

				if (dir==0) {
					system("sudo /home/pi/www/./move_in");
				} else {
					system("sudo /home/pi/www/./move_out");
				}
			
				system("sudo /home/pi/www/./focus_update");
				
				pt=fopen("/home/pi/www/current_pos.txt","r");
				fgets(step,10,pt);
				fclose(pt);
				
				pt = fopen("vcurve.txt", "a"); // Open File
				fprintf(pt,"+++++%s\n",step);
				fclose(pt); // Close File	
				
				sleep(delay);
				
			}
			
			

	}
	
	pt = fopen("vcurve.txt", "a"); // Open File
	fprintf(pt,"__________\n");
	fclose(pt); // Close File	
	
}

