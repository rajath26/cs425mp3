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

int tcp, udp; 

void * startKelsa(void *threadNum)
{
    int *counter;


printf("\nIN thread func\n");
    counter = (void *) threadNum;

    printf("\nCOUNTER %d\n", *counter);

    switch(*counter)
    {
        case 0: 
            printf("\nStarting UDP thread\n");
            startUDP(); 
            break;
 
        case 1: 
            printf("\nStarting TCP thread\n");
            startTCP();
            break;

        default:
            break;
    }
}

int startUDP()
{

    char buffer[4096];

    int len;

    struct sockaddr_in clientAddress;

    len = sizeof(clientAddress);

  printf("\n Waiting in UDP\n");

    //for(;;)
    //{

    printf("\n Waiting in UDP\n");

    recvfrom(udp, buffer, 4096, 0, (struct sockaddr *) &clientAddress, &len);

    printf("\nI AM HERE IN UDP AND RECEIVED %s FROM CLIENT\n", buffer);
   //}

    return 0;



}

int startTCP()
{
    int sd, len;

    struct sockaddr_in clientAddress;

    char buffer[4096];

    len = sizeof(clientAddress);

    printf("\n Waiting in TCP\n");

    //for(;;)
   //{

   printf("\n Waiting in TCP\n");

   //listen(tcp, 10);

    sd = accept(tcp, (struct sockaddr *) &clientAddress, &len);
    if ( sd < 0 ) 
    {
        printf("\nPrintf TCP accept failure\n");
    }

    recv(sd, buffer, 4096, 0);

    printf("\nI AM IN HERE IN TCP AND RECEIVED %s FROM CLIENT\n", buffer);

   //}

    return 0;

}

int main(int argc, char *argv[])
{

   int i_rc, counter, *ptr[2];

   struct sockaddr_in serverAddress, serverAddress1;

   pthread_t tid[2];

   tcp = socket (AF_INET, SOCK_STREAM, 0);
   if ( tcp < 0 )
   { 
       printf("\nTCP socket error\n");
       return -1; 
   }

   printf("\nTCP socket success");

   udp = socket(AF_INET, SOCK_DGRAM, 0);
   if ( udp < 0 )
   {
       printf("\nUDP socket error\n");
       return -1;
   }

   printf("\nUDP socket success\n");

   memset(&serverAddress, 0, sizeof(struct sockaddr_in));
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(3491);
   serverAddress.sin_addr.s_addr = inet_addr("192.168.139.134");
   memset(&(serverAddress.sin_zero), '\0', 8);

   /*memset(&serverAddress1, 0, sizeof(struct sockaddr_in));
   serverAddress1.sin_family = AF_INET;
   serverAddress1.sin_port = htons(3491);
   serverAddress1.sin_addr.s_addr = inet_addr("192.168.139.134");
   memset(&(serverAddress1.sin_zero), '\0', 8);*/

   // Bind 
   i_rc = bind(udp, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
   if ( -1 == i_rc )
   {
      printf("\nUnable to bind UDP\n");
      return -1;
   }

   printf("\n UDP bind success");

   i_rc = bind(tcp, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
   if ( -1 == i_rc ) 
   {
       printf("\nUnable to bind TCP\n"); 
       return -1;
   }

   printf("\n TCP bind success");

   listen(tcp, 10);

   for (counter = 0;  counter < 2; counter++)
   {
      ptr[counter] = (int *)malloc(sizeof(int));
      *(ptr[counter]) = counter; 

      printf("\nCOUNTER IN MAIN : %d\n", counter);
      
      i_rc = pthread_create(&tid[counter], NULL, startKelsa, (void *) ptr[counter]);
      if ( 0 != i_rc )
      {
          printf("\npthread creation failure\n");
      }
      else 
      {
          printf("\npthread creation successful in main\n");
      }

    }

      
 
      for (counter = 0; counter < 2; counter++) 
      {
          pthread_join(tid[counter], NULL);
      }
   

} // End of main()
