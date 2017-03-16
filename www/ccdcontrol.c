#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){

	int filter,i_filter,n_filter;
	int n_frames;
	int delay;
	int i;
	int k;

	FILE *pt;
	
	char buff[10];
	
	pt=fopen("/home/pi/www/ccd_filter.txt","r");
	fscanf(pt,"%d",&filter);
	fclose(pt);

	pt=fopen("/home/pi/www/ccd_nf.txt","r");
	fscanf(pt,"%d",&n_frames);
	fclose(pt);
	
	pt=fopen("/home/pi/www/ccd_delay.txt","r");
	fscanf(pt,"%d",&delay);
	fclose(pt);
	
	pt = fopen("status_ccd", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	pt = fopen("image.txt", "w"); // Open File
	fprintf(pt,"");
	fclose(pt); // Close File
	
	switch(filter)
	{
	case 13:
		i_filter=1;
		n_filter=3;
		break;
	case 23:
		i_filter=2;
		n_filter=3;
		break;
	case 14:
		i_filter=1;
		n_filter=4;
		break;
	case 24:
		i_filter=2;
		n_filter=4;
		break;
	case 34:
		i_filter=3;
		n_filter=4;
		break;
	default:
		i_filter=filter;
		n_filter=filter;
	}
	
	while (i_filter<=n_filter) {

		switch(i_filter)
		{
		case 1:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #1\n");
			fclose(pt);
			system("sudo /home/pi/www/./F1");
			break;
		case 2:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #2\n");
			fclose(pt);
			system("sudo /home/pi/www/./F2");
			break;
		case 3:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #3\n");
			fclose(pt);
			system("sudo /home/pi/www/./F3");
			break;
		case 4:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #4\n");
			fclose(pt);
			system("sudo /home/pi/www/./F4");
			break;
		case 5:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #5\n");
			fclose(pt);
			system("sudo /home/pi/www/./F5");
			break;
        case 6:
            pt = fopen("image.txt", "a");
            fprintf(pt,"Filter #6\n");
            fclose(pt);
            system("sudo /home/pi/www/./F6");
            break;
        case 7:
            pt = fopen("image.txt", "a");
            fprintf(pt,"Filter #7\n");
            fclose(pt);
            system("sudo /home/pi/www/./F7");
            break;
        case 8:
            pt = fopen("image.txt", "a");
            fprintf(pt,"Filter #8\n");
            fclose(pt);
            system("sudo /home/pi/www/./F8");
            break;
		case 0:
			pt = fopen("image.txt", "a");
			fprintf(pt,"No Filter Change\n");
			fclose(pt);
			sleep(3);
		}
	
		i=1;
		while (i<=n_frames) {
			
			system("echo RUN: > /tmp/gtkImager");
			
			pt = fopen("status_capture", "w"); // Open File
			fprintf(pt,"");
			fclose(pt); // Close File
			
			sleep(1);
			k=1;
		
			while(access("/home/pi/www/status_capture",F_OK) == 0) 
			{
				
				system("tail -1 CCD_log.txt > check_status_capture.txt");
				pt = fopen("check_status_capture.txt", "r"); // Open File
				fgets (buff,10,pt);
			    fclose(pt); // Close File
				if ( (buff[6]=='P' && buff[7]=='R') || (buff[6]=='C' && buff[7]=='A') ){
					system("rm status_capture");
				}
				
				// Detect error
				if ( (buff[6]=='E' && buff[7]=='R') ){
					pt = fopen("image.txt", "a"); // Open File
					fprintf(pt,"Error while capturing an image!\n");
					fclose(pt); // Close File					
					sleep(2);
					system("rm status_capture");
					system("rm status_ccd");
					return(0);
				}
				
                pt = fopen("image.txt", "a"); // Open File
				if (i_filter==0) {
					fprintf(pt,"Frame %d of %d : Elapsed time = %d sec\n",i,n_frames,k);
				} else {
					fprintf(pt,"Filter #%d, Frame %d of %d : Elapsed time = %d sec\n",i_filter,i,n_frames,k);
				}
                fclose(pt); // Close File
			
				sleep(1);
				k=k+1;
				
			}
			
			pt = fopen("image.txt", "a"); // Open File
			if (i_filter==0) {
				fprintf(pt,"Frame %d of %d : Saving image\n",i,n_frames);
			} else {
				fprintf(pt,"Filter #%d, Frame %d of %d : Saving image\n",i_filter,i,n_frames);
			}
			fclose(pt); // Close File
			
			sleep(3);
			
			system("cp -n /home/pi/www/tmp_images/*.fit /home/pi/www/images/.");
			system("cp -n /home/pi/www/tmp_images/*.jpg /home/pi/www/images/.");
			system("cp /home/pi/www/tmp_images/*.jpg /home/pi/www/image.jpg");
			system("cp /home/pi/www/tmp_images/*.txt /home/pi/www/histogram.txt");
			system("rm /home/pi/www/tmp_images/*.*");
			system("perl histogram_split.pl");
			
			pt = fopen("image.txt", "a"); // Open File
			if (i_filter==0) {
				fprintf(pt,"Frame %d of %d : Completed\n",i,n_frames);
			} else {
				fprintf(pt,"Filter #%d, Frame %d of %d : Completed\n",i_filter,i,n_frames);
			}
			fclose(pt); // Close File
			
			pt = fopen("/home/pi/www/speak", "w");
			if (i_filter==0) {
				fprintf(pt,"Frame %d of %d was completed\n",i,n_frames);
			} else {
				fprintf(pt,"Filter %d, Frame %d of %d was completed\n",i_filter,i,n_frames);
			}
			fclose(pt);	
						
			sleep(2);
			i=i+1;
			
			if (n_frames>1){
				if(access("/home/pi/www/dithering",F_OK) == 0)
				{
					pt = fopen("image.txt", "a"); // Open File
					fprintf(pt,"Dithering...\n");
					fclose(pt); // Close File
					system("sudo /home/pi/www/./remote_client 4");
				}
				sleep(delay);
			}
			
		}
		
		i_filter=i_filter+1;
	
	}	
	
	pt = fopen("image.txt", "a"); // Open File
	fprintf(pt,"Done\n");
	fclose(pt); // Close File
	
	//pt = fopen("/home/pi/www/speak", "w");
	//fprintf(pt,"Job completed\n");
	//fclose(pt);
		
	sleep(2);
	
	system("rm status_ccd");

	if(access("/home/pi/www/notify_imaging",F_OK) == 0)
	{
		system("perl /home/pi/www/notify_imaging.pl");
	}
	
}

