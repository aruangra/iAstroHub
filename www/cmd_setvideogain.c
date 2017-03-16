/*
 * remote_client.c
 *
 *      Author: gm
 *
 *
 * This file is part of Lin_guider.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// client in the unix domain for Lin_guider
// gcc -Wall -ggdb3 -O0 -o remote_client remote_client.c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define HDR_SZ 8

enum commands
{
  GET_VER = 1,
  SET_SQUARE_POS,
  SAVE_FRAME,
  DITHER,
  
		START_AUTOGUIDING, 
		STOP_AUTOGUIDING,
        START_CALIBRATION,
        STOP_CALIBRATION,
        FINDSTAR,
		SET_VIDEO_GAIN,  //used 
		SET_FPS,         //used 
		SET_GAIN_P,      //used 
		GET_GAIN_P,
		MOVE_RETICLE,
  
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

	int cmd = 10;
	   
	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, "/tmp/lg_ss");
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
	if( (sockfd = socket(AF_UNIX, SOCK_STREAM,0)) < 0 )
		error("Error Creating socket");
	if( connect(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0 )
		error("Error Connecting to Lin_guider");
	printf("Connected to Lin_guider\n");
	//printf("Enter command params: ");
	bzero(buffer,82);
	
    pFile = fopen ("/home/pi/www/guidestar/set_videogain.txt" , "r");
    fgets (buffer, 80, pFile);
    fclose(pFile);
	
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
