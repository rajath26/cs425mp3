//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: KVclient.h 
//
//    DECSRIPTION: This is the header file for the client of the 
//                 key-value store
//
//    OPERATING SYSTEM: Linux UNIX only
//    TESTED ON:
//
//    CHANGE ACTIVITY:
//    Date        Who      Description
//    ==========  =======  ===============
//    11-09-2013  Rajath   Initial creation
//
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////

/*
 * Header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

/*
 * Macros
 */
#define SUCCESS                   0
#define ERROR                     -1
#define CLIENT_NUM_OF_CL_ARGS     5 
#define NUM_OF_THREADS            2

/*
 * Global variables
 */
int client_tcp;
char clientPortNo[SMALL_BUF_SZ];
char clientIpAddr[SMALL_BUF_SZ];
char serverPortNo[SMALL_BUF_SZ];
char KVclientCmd[LONG_BUF_SZ];
char opCode[SMALL_BUF_SZ];
char key[SMALL_BUF_SZ];
char value[LONG_BUF_SZ];
char * msgToSend;
struct sockaddr KVClientAddr;

/*
 * Function Declarations
 */
int KVClient_CLA_check(int argc, char *argv[]);
int setUpTCP(char * portNo, char * ipAddress);
int spawnHelperThreads();
void * startKelsa(void *threadNum);
int clientReceiveFunc();
int clientSenderFunc();
int parseKVClientCmd();
int createAndSendOpMsg();

/*
 * END
 */
