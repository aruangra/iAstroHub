// client in the unix domain for Lin_guider
// gcc -Wall -ggdb3 -O0 -o remote_client remote_client.c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

#define HDR_SZ 8

enum commands
{
  GET_VER = 1,
  SET_SQUARE_POS,
  SAVE_FRAME,

  CMD_MAX
};

void error(char *);

int main(int argc, char *argv[])
{
   int sockfd, servlen;
   struct sockaddr_un  serv_addr;
   char buffer[82];
   char readbuf[16384];
   FILE * pFile;

//   if( argc < 2 )
//   {
//  	   printf("Usage: client [command no]\nThen type parameters in console and press 'Enter'\n\
//\t\t'GET_VER' = 1, no params\n\
//\t\t'SET_SQUARE_POS'=2, [x y]\n\
//\t\t'SAVE_FRAME'=3, file_name w/o extension\n\
//");
//  	   return 0;
//   }
   int cmd = 17;
//   if( cmd < GET_VER || cmd >= CMD_MAX )
//   {
//  	   printf("Wrong command number\n");
//  	   return 0;
//   }

   bzero((char *)&serv_addr,sizeof(serv_addr));
   serv_addr.sun_family = AF_UNIX;
   strcpy(serv_addr.sun_path, "/tmp/lg_ss");
   servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
   if( (sockfd = socket(AF_UNIX, SOCK_STREAM,0)) < 0 )
       error("Error Creating socket");
   if( connect(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0 )
       error("Error Connecting to Lin_guider");
   printf("***** Connected to Lin_guider\n");
   //printf("Enter command params: ");
   bzero(buffer,82);

   pFile = fopen ("/home/pi/www/guidestar/image.txt" , "r");
   fgets (buffer, 80, pFile);
   fclose(pFile);

   //fgets(buffer,80,stdin);
   //for( i = 0;i < 100;i++ )
   {
   		char out[256];
		uint16_t *ptr16 = (uint16_t *)out;
		uint32_t *ptr32 = (uint32_t *)out;

		ptr16[0] = 2;	// SIGNATURE
		ptr16[1] = cmd;		// CMD
		ptr32[1] = strlen(buffer)-1;	// DATA LEN
		memcpy( out+HDR_SZ, buffer, strlen(buffer)-1 );

		{
		int pos = 0;
        int n = 0;
        int to_write = HDR_SZ+strlen(buffer)-1;
        do
        {
            n = write( sockfd, out+pos, to_write );
            if( n < 0 )
                error("ERROR writing to socket");
            pos += n;
        }while( pos < to_write );
		printf("Wrote\n");
		}
		
		{
		int pos = 0;
        int n = 0;
        int to_read = HDR_SZ;
		do
  	    {
      		n = read( sockfd, readbuf+pos, to_read );
      		pos += n;
      		if( n == to_read )
      		{
      			int data_sz = ((uint32_t*)(readbuf+4))[0];
      			printf("answer data size = %d\n", data_sz);
      			if( data_sz < 0 || data_sz > 16000 )
      				error("ERROR length to read");
      			to_read += data_sz;
      		}
  		}while( pos < to_read );
		printf("The return message was '%.*s'\n", ((uint32_t*)(readbuf+4))[0], readbuf+HDR_SZ);
		}
   }
   printf("=== done ===\n");
   //sleep(2);
   close(sockfd);
   
   return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}
