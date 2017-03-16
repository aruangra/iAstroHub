#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ( int argc, char *argv[] )
{

	int i=atoi(argv[1]);
	int n_frames=atoi(argv[2]);
	int exposure=atoi(argv[3]);
	int k;

	FILE *pt;

	for(k=1;k<=exposure;k++)
	{
		sleep(1);
	        pt = fopen("/home/pi/www/photo.txt", "a"); // Open File
        	fprintf(pt,"Frame %d of %d : Exposing %d / %d s\n",i,n_frames,k,exposure);
        	fclose(pt); // Close File
	}

}

