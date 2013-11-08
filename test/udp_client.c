#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 3491

int main (int argc, char *argv[])
{
    
    int sd,
        rc,
        i;
  
    char buffer[1024] = "hello world";

    struct sockaddr_in clientAddress,
                       serverAddress;

    if (argc != 2)
    {
        printf("\nInvalid arguments\n");
        return 1;    
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sd )
    {
        printf("\nUnable to open socket\n");
        return 1;
    }

    memset(&clientAddress, 0, sizeof(struct sockaddr_in));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(3493);
    clientAddress.sin_addr.s_addr = inet_addr("192.168.139.134");
    memset(&(clientAddress.sin_zero), '\0', 8);

    rc = bind(sd, (struct sockaddr *) &clientAddress, sizeof(clientAddress)); 
    if ( -1 == rc )
    {
        printf("\nUnable to bind: %d\n", errno);
        perror("bind");
        return 1;
    }
    
    printf("\nEnter string to be sent\n");
//    scanf("%s",buffer); 
    printf("before meme");
    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(3491);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.139.134");
    memset(&(serverAddress.sin_zero), '\0', 8);

    char *temp = buffer;
    printf("i am here\n");
    for (i = 0; i < 10; i++){
    sendto(sd, temp, sizeof(buffer), 0, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr));}

    close(sd);

} // End of main()
