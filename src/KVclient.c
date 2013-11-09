/////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: KVclient.c
//
//    DECSRIPTION: This is the source file for the client of the 
//                 key value store 
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

#include "../inc/KVclient.h"

/* 
 * Function defintions
 */ 

/*****************************************************************
 * NAME: KVClient_CLA_check
 *
 * DESCRIPTION: This function is designed to check if command 
 *              line arguments is valid 
 *              
 * PARAMETERS: 
 *            (int) argc - number of command line arguments
 *            (char *) argv - three command line arguments apart 
 *                            from argv[0] namely:
 *                            i) port no
 *                            ii) Ip Address of host
 *                            iii) port no of server 
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int KVClient_CLA_check(int argc, char *argv[])
{

    int rc = SUCCESS;

    if ( argc != CLIENT_NUM_OF_CL_ARGS )
    {
        printf("\nInvalid usage\n");
        printf("\nUsage information: %s <port_no> <ip_address> <server_port>", argv[0]);
        rc = ERROR;
        goto rtn;
    }

  rtn:
    return rc;

} // End of KVClient_CLA_check()

/*****************************************************************
 * NAME: setUpTCP 
 *
 * DESCRIPTION: This function is designed to create a TCP socke 
 *              and bind to the port 
 *              
 * PARAMETERS: 
 *            (char *) portNo: port number
 *            (char *) ipAddress: IP Address
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int setUpTCP(char * portNo, char * ipAddress)
{

    int rc = SUCCESS;

    client_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if ( ERROR == tcp )
    {
        printf("\nUnable to open socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("socket");
        rc = ERROR;
    }

    memset(&KVClientAddr, 0, sizeof(struct sockaddr_in));
    KVClientAddr.sin_family = AF_INET;
    KVClientAddr.sin_port = htons(atoi(portNo));
    KVClientAddr.sin_addr.s_addr = inet_addr(ipAddress);
    memset(&(KVClientAddr.sin_zero), '\0', 8);

    i_rc = bind(client_tcp, (struct sockaddr *) &KVClientAddr, sizeof(KVClientAddr));
    if ( ERROR == i_rc )
    {
        printf("\nUnable to bind TCP socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("bind");
        printToLog(logF, ipAddress, "TCP bind() failure");
        rc = ERROR;
        goto rtn;
    }

    listen(client_tcp, 10);

} // End of setUpTCP()

/*****************************************************************
 * NAME: spawnHelperThreads 
 *
 * DESCRIPTION: This function spawns helper threads 
 *              
 * PARAMETERS: NONE
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int spawnHelperThreads()
{

    int rc = SUCCESS,                    // Return code
        i_rc,                            // Temp RC
        threadNum = 0,                   // Thread counter
        *ptr[NUM_OF_THREADS];            // Pointer to thread counter

    register int counter;                // Counter variable

    pthread_t threadID[NUM_OF_THREADS];  // Helper threads

    /*
     * Create threads:
     */
    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        ptr[counter] = (int *) malloc(sizeof(int));
        *(ptr[counter]) = counter;

        i_rc = pthread_create(&threadID[counter], NULL, startKelsa, (void *) ptr[counter]);
        if ( SUCCESS != i_rc )
        {
            printf("\npthread creation failure\n");
            printf("\nError ret code: %d, errno: %d\n", i_rc, errno);
            printf("\nExiting.... ... .. . . .\n");
            rc = ERROR;
            goto rtn;
        }
    }

    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        pthread_join(threadID[counter], NULL);
    }

  rtn:
    return rc;
} // End of spawnHelperThreads()

/****************************************************************
 * NAME: startKelsa 
 *
 * DESCRIPTION: This is the pthread function 
 *              
 * PARAMETERS: 
 * (void *) threadNum - thread counter
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
void * startKelsa(void *threadNum)
{

    funcEntry(logF, ipAddress, "startKelsa");

    int rc = SUCCESS,                 // Return code
        i_rc,                         // Temp RC
        *counter;                     // Thread counter

    //counter = (int *) malloc(sizeof(int));
    counter = (int *) threadNum;

    pthread_t tid = pthread_self();   // Thread ID

    switch(*counter)
    {
        case 0:
        // First thread displays KV functionalities
        // and sends request to local server
        i_rc = clientSenderFunc();
        break;

        case 1:
        // Second thread receives response from servers
        // and prints them
        i_rc = clientReceiveFunc();
        break;

        default:
        printf("\nDefault CASE. An ERROR\n");
        rc = ERROR;
        goto rtn;
        break;
    }

  rtn:
    return rc

} // End of startKelsa()

/****************************************************************
 * NAME: clientReceiveFunc 
 *
 * DESCRIPTION: This is the function that takes care:
 *              i) Receiving response from server
 *              ii) Extracting message to determine result op
 *                  code
 *              iii) Print the result
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int clientReceiveFunc()
{

    int rc = SUCCESS,
        numOfBytesRec;

    char recMsg[LONG_BUF_SZ];

    struct sockaddr_in hostAddress();

    struct op_code *temp = NULL;

    for (;;)
    {

        memset(recMsg, '\0', LONG_BUF_SZ);
        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
        numOfBytesRec = 0;
        temp = NULL;

        numOfBytesRec = recvTCP(recMsg, LONG_BUF_SZ, &serverAddress);
        // Check if 0 bytes is received 
        if ( SUCCESS == numOfBytesRec )
        {
            printf("\nNumber of bytes received is ZERO = %d\n", numOfBytesRec);
            continue;
        }

        i_rc = extract_message_op(recMsg, &temp);
        if ( ERROR == i_rc )
        {
            printf("\nUnable to extract received message. Return code of extract_message_op = %d\n", i_rc);
             continue;
        }

        switch( temp->opcode )
        { 
            case LOOKUP_RESULT:
                printf("\nSUCCESSFUL LOOKUP

            break;

            case INSERT_RESULT:
        
            break;
 
            case DELETE_RESULT:

            break;

            case UPDATE_RESULT:
 
            break;

            case ERROR:

            break;

            default:
                // We shouldn't be here 
                printf("\nDefault case in switch\n");
                continue;
        } // End of switch( temp->opcode )

    } // End of for(;;)

  rtn:
    return rc;

} // End of clientReceiveFunc()


/*
 * Main function
 */ 

/*****************************************************************
 * NAME: main 
 *
 * DESCRIPTION: Main function of the client of the key-value store 
 *              
 * PARAMETERS: 
 *            (int) argc - number of command line arguments
 *            (char *) argv - two command line arguments apart 
 *                            from argv[0] namely:
 *                            i) Port No of client
 *                            ii) Ip Address of host
 *                            iii) Port no of server
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int main(int argc, char *argv[])
{

    int rc = SUCCESS,                      // Return code
        i_rc;                              // Temp RC

    char clientIpAddress[SMALL_BUF_SZ],    // Client IP address
         clientPortNo[SMALL_BUF_SZ],       // Client Port No
         FEPortNo[SMALL_BUF_SZ];           // Front End Port No

    /*
     * Commannd line arguments check 
     */ 
    i_rc = KVClient_CLA_check(argc, argv);
    if ( i_rc != SUCCESS )
    {
        rc ERROR;
        goto rtn;
    }

    /*
     * Copy ip address, port no and server port no
     */
    memset(clientPortNo, '\0', SMALL_BUF_SZ);
    strcpy(clientPortNo, argv[1]);
    memset(clientIpAddr, '\0', SMALL_BUF_SZ);
    strcpy(clientIpAddr, argv[2]);
    memset(serverPortNo, '\0', SMALL_BUF_SZ); 

    /*
     * Set up TCP 
     */
    i_rc = setUpTCP(clientPortNo, clientIpAddress);
    if ( i_rc != SUCCESS )
    {
         rc = ERROR;
         goto rtn;
    }

    /*
     * Spawn helper threads
     */
    i_rc = spawnHelperThreads();
    if ( i_rc != SUCCESS )
    {
        rc = ERROR; 
        goto rtn;
    }

  rtn:
    return rc;

} // End of main()


