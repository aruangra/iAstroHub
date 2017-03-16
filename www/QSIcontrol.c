#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

int isDirectoryEmpty();

int main (){

	int filter,i_filter,n_filter;
	int n_frames;
	int delay;
	int i;
	int k;

	FILE *pt;
	
	char buff[50];
	
	pt=fopen("/home/pi/www/ccd_filter.txt","r");
	fscanf(pt,"%d",&filter);
	fclose(pt);

	pt=fopen("/home/pi/www/ccd_nf.txt","r");
	fscanf(pt,"%d",&n_frames);
	fclose(pt);
	
	pt=fopen("/home/pi/www/ccd_delay.txt","r");
	fscanf(pt,"%d",&delay);
	fclose(pt);
	
//	pt = fopen("status_ccd", "w"); // Open File
//	fprintf(pt,"");
//	fclose(pt); // Close File
	
//	pt = fopen("image.txt", "w"); // Open File
//	fprintf(pt,"");
//	fclose(pt); // Close File
	
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
			system("echo 'cmbFilType R' > /home/pi/www/Commands.txt");
			break;
		case 2:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #2\n");
			fclose(pt);
			system("echo 'cmbFilType G' > /home/pi/www/Commands.txt");
			break;
		case 3:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #3\n");
			fclose(pt);
			system("echo 'cmbFilType B' > /home/pi/www/Commands.txt");
			break;
		case 4:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #4\n");
			fclose(pt);
			system("echo 'cmbFilType Lum' > /home/pi/www/Commands.txt");
			break;
		case 5:
			pt = fopen("image.txt", "a");
			fprintf(pt,"Filter #5\n");
			fclose(pt);
			system("echo 'cmbFilType Ha' > /home/pi/www/Commands.txt");
			break;
		case 0:
			pt = fopen("image.txt", "a");
			fprintf(pt,"No Filter Change\n");
			fclose(pt);
		}
		sleep(2);
	
		i=1;
		while (i<=n_frames) {

			pt = fopen("status_capture", "w"); // Open File
			fprintf(pt," ");
			fclose(pt); // Close File
		
			system("echo btnCCDStart > /home/pi/www/Commands.txt");
			
			sleep(1);
			k=1;
		
			while(access("/home/pi/www/status_capture",F_OK) == 0) 
			{
				
				if (isDirectoryEmpty()==0){
					system("rm status_capture");
				}
				
                pt = fopen("image.txt", "a"); // Open File
				if (i_filter==0) {
					fprintf(pt,"Frame %d of %d : Elapsed time = %d s\n",i,n_frames,k);
				} else {
					fprintf(pt,"Filter #%d, Frame %d of %d : Elapsed time = %d s\n",i_filter,i,n_frames,k);
				}
                fclose(pt); // Close File
			
				sleep(1);
				k=k+1;
				
			}

			pt = fopen("image.txt", "a"); // Open File
			if (i_filter==0) {
				fprintf(pt,"Frame %d of %d : Preparing preview image\n",i,n_frames);
			} else {
				fprintf(pt,"Filter #%d, Frame %d of %d : Preparing preview image\n",i_filter,i,n_frames);
			}
			fclose(pt); // Close File
			
			
			system("sudo sh /home/pi/www/process_image.sh");			
			
			
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
	
	if(access("/home/pi/www/focuserlink",F_OK) == 0)
	{
		system("echo 'open_focuser_comms 0' > /home/pi/www/Commands.txt");
	}

	if(access("/home/pi/www/notify_imaging",F_OK) == 0)
	{
		system("perl /home/pi/www/notify_imaging.pl");
	}
	
	
}

int isDirectoryEmpty() {
  int n = 0;
  struct dirent *d;
  DIR *dir = opendir("/home/pi/www/tmp_images/");
  if (dir == NULL) //Not a directory or doesn't exist
    return 1;
  while ((d = readdir(dir)) != NULL) {
    if(++n > 2)
      break;
  }
  closedir(dir);
  if (n <= 2) //Directory Empty
    return 1;
  else
    return 0;
}

