#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

int main (){

	int n_frames;
	int exposure;
	int delay;
	int iso;
	int index;
	int i;
	int k;
	char cmd[150];
	char outname[50];
	time_t curtime;
        struct tm *loctime;

	FILE *pt;

	pt=fopen("/home/pi/www/dslr_nf.txt","r");
	fscanf(pt,"%d",&n_frames);
	fclose(pt);

	if (n_frames < 1){
		n_frames=1;
	}

	pt=fopen("/home/pi/www/dslr_ex.txt","r");
	fscanf(pt,"%d",&exposure);
	fclose(pt);

	if (exposure < 0){
		exposure=0;
	}

	pt=fopen("/home/pi/www/dslr_de.txt","r");
	fscanf(pt,"%d",&delay);
	fclose(pt);

	if (delay < 16){
		delay=16;
	}
	
	pt=fopen("/home/pi/www/dslr_iso.txt","r");
	fscanf(pt,"%d",&iso);
	fclose(pt);

	if (iso < 500){
		iso=500;
	}

	pt=fopen("/home/pi/www/dslr_ou.txt","r");
	fscanf(pt,"%s",outname);
	fclose(pt);

	pt = fopen("/home/pi/www/status_dslr", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File

	pt = fopen("/home/pi/www/photo.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File

	sleep(2);

		i=1;
		while (i<=n_frames) {

			curtime = time (NULL);
			loctime = localtime (&curtime);
			pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
			fprintf(pt, "*** ");
			fputs (asctime (loctime), pt);
			fprintf(pt,"Frame %d of %d started\n",i,n_frames);
			fclose(pt); // Close File

			pt = fopen("/home/pi/www/speak", "w");
			fprintf(pt,"Frame %d started\n",i);
			fclose(pt);
			
			
			
			
			// Photo index
			pt=fopen("/home/pi/www/dslr_index.txt","r");
			fscanf(pt,"%d",&index);
			fclose(pt);
			index=index+1;
			pt = fopen("/home/pi/www/dslr_index.txt", "w");
			fprintf(pt,"%d",index);
			fclose(pt);
			
			// Shutter control
			sprintf(cmd,"sudo bash /home/pi/www/shutter.sh %d %d %d %d %s %d",i,n_frames,exposure,iso,outname,index);
			system(cmd);
			
			

			curtime = time (NULL);
			loctime = localtime (&curtime);
			pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
			fprintf(pt, "*** ");
			fputs (asctime (loctime), pt);
			fprintf(pt,"Frame %d of %d completed\n",i,n_frames);
			fclose(pt); // Close File

			pt = fopen("/home/pi/www/speak", "w");
			fprintf(pt,"Frame %d completed\n",i);
			fclose(pt);

			//Dithering
			sleep(delay-7);

			while(access("/home/pi/www/status_loading",F_OK) == 0) 
			{
				sleep(1);
			}

			i=i+1;
			
			if(access("/home/pi/www/dithering",F_OK) == 0 && i<=n_frames)
			{
				pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
				fprintf(pt,"Dithering... Next frame will start soon.\n");
				fclose(pt); // Close File
				system("sudo /home/pi/www/./remote_client 4");
			sleep(2);
			}			

		}

	pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
	fprintf(pt,"Done\n");
	fclose(pt); // Close File

	pt = fopen("/home/pi/www/speak", "w");
	fprintf(pt,"Job completed\n");
	fclose(pt);

	system("rm /home/pi/www/status_dslr");

}
