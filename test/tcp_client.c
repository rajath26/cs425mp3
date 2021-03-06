#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<pthread.h>
#include<sys/time.h>

int main(int argc, char *argv[])
{

     struct sockaddr_in serverAddress; 

     int sd; 


     sd = socket(AF_INET, SOCK_STREAM, 0);
     if ( sd < 0 ) 
     {
          printf("\n TCP socket creation failure\n");
          return -1;
     } 

     memset( &(serverAddress), 0, sizeof(serverAddress));
     serverAddress.sin_family = AF_INET;
     serverAddress.sin_port = htons(3491);
     serverAddress.sin_addr.s_addr = inet_addr("192.168.139.134");
     if (connect(sd,(struct sockaddr*)&serverAddress, sizeof(serverAddress))<0)
	{
		printf("Cannot Connect to server");
		exit(1);
	}

     send(sd, "hello world", 20, 0);

     return 0;
}
