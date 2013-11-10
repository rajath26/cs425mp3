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
#include "key_value_store.c"
#include "tcp.c"

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
 *                            iv) KV Client Command
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
        printf("\nUsage information: %s <port_no> <ip_address> <server_port> <KV_client_command>\n", argv[0]);
        printf("\t\t-> KV Functionalities / OP_CODE\n");
        printf("\t\ti) INSERT\n"); 
        printf("\t\tii) LOOKUP\n");
        printf("\t\tiii) UPDATE\n");
        printf("\t\tiv) DELETE\n");
        printf("\t\t-> COMMAND FORMAT\n");
        printf("\t\t<OP_CODE>:::<key>:::<value>\n");
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

    int rc = SUCCESS,
        i_rc;

    tcp = socket(AF_INET, SOCK_STREAM, 0);
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

    i_rc = bind(tcp, (struct sockaddr *) &KVClientAddr, sizeof(KVClientAddr));
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

    listen(tcp, LISTEN_QUEUE_LENGTH);

  rtn:
    return rc;

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
        break;
    }

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
        numOfBytesRec,
        i_rc;

    char recMsg[LONG_BUF_SZ];

    struct sockaddr_in serverAddress;

    struct op_code *temp = NULL;

    // The design is to execute client each time for each operation

        memset(recMsg, '\0', LONG_BUF_SZ);
        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
        numOfBytesRec = 0;
        temp = NULL;

        numOfBytesRec = recvTCP(recMsg, LONG_BUF_SZ, serverAddress);
        // Check if 0 bytes is received 
        if ( SUCCESS == numOfBytesRec )
        {
            printf("\nNumber of bytes received is ZERO = %d\n", numOfBytesRec);
            rc = ERROR;
            goto rtn; 
        }

        i_rc = extract_message_op(recMsg, &temp);
        if ( ERROR == i_rc )
        {
            printf("\nUnable to extract received message. Return code of extract_message_op = %d\n", i_rc);
            rc = ERROR;
            goto rtn; 
        }

        switch( temp->opcode )
        { 
            case LOOKUP_RESULT:
                printf("\n");
                printf("\t\t*****RESULT*****\n");
                printf("\t\tSUCCESSFUL LOOKUP\n");
                printf("\t\tKEY: %d - VALUE: %s\n", temp->key, temp->value);
                printf("\t\t****************\n");
            break;

            case INSERT_RESULT:
                printf("\n");
                printf("\t\t*****RESULT*****\n");
                printf("\t\tSUCCESSFUL INSERT\n");
                printf("\t\t****************\n");
            break;
 
            case DELETE_RESULT:
                printf("\n");
                printf("\t\t*****RESULT*****\n");
                printf("\t\tSUCCESSFUL DELETE\n");
                printf("\t\t****************\n");
            break;

            case UPDATE_RESULT:
                printf("\n");
                printf("\t\t*****RESULT*****\n");
                printf("\t\tSUCCESSFUL UPDATE\n");
                printf("\t\t****************\n");
            break;

            case ERROR_RESULT:
                printf("\n");
                printf("\t\t*****RESULT*****\n");
                printf("\t\tERROR DURING OPERATION. TRY AGAIN\n");
                printf("\t\t****************\n");
            break;

            default:
                // We shouldn't be here 
                printf("\nGot something else apart from result and I am not supposed to be getting this. Damn it!!!! Default case in switch\n");
                rc = ERROR;
                goto rtn;
        } // End of switch( temp->opcode )

  rtn:
    return rc;

} // End of clientReceiveFunc()

/****************************************************************
 * NAME: clientSenderFunc 
 *
 * DESCRIPTION: This is the function that takes care:
 *              i) Parse the KV client command 
 *              ii) create message based on choice 
 *              iii) choose peer node to send the request
 *              iv) send the request
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int clientSenderFunc()
{

    int rc = SUCCESS,
        i_rc;

    // Parse the received KV client command 
    i_rc = parseKVClientCmd();
    if ( ERROR == i_rc ) 
    {
        printf("\nUnable to parse Daisy KV Client Command\n");
        rc = ERROR;
        goto rtn;
    }

    // Create message based on OP CODE
    i_rc = createAndSendOpMsg();
    if ( ERROR == i_rc )
    {
        printf("\nUnable to create and send message based on op code\n");
        rc = ERROR;
        goto rtn;
    }

  rtn:
    return rc;

} // End of clientSenderFunc()

/****************************************************************
 * NAME: parseKVClientCmd 
 *
 * DESCRIPTION: This function parses the KV client command and from it
 *              tokenizes the OP_CODE, key and value 
 *              -> KV Functionalities / OP_CODE
 *              i) INSERT
 *              ii) LOOKUP
 *              iii) UPDATE
 *              iv) DELETE
 * 
 *              -> COMMAND FORMAT
 *              <OP_CODE>:::<key>:::<value>
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int parseKVClientCmd()
{

    int rc = ERROR;

    int opCodeSet = 0,
        keySet = 0,
        valueSet = 0,
        insert = 0,
        lookup = 0,
        update = 0,
        delete = 0,
        invalidOpCode = 0;

    char *token; 

    // OP CODE
    token = strtok(KVclientCmd, ":::");
    if ( token != NULL )
    { 
        strcpy(opCode, token);
        opCodeSet = 1;
        if ( (strcasecmp(opCode, "INSERT") == 0) )
            insert = 1;
        else if ( SUCCESS == strcasecmp(opCode, "LOOKUP") )
            lookup = 1;
        else if ( SUCCESS == strcasecmp(opCode, "UPDATE") )
            update = 1;
        else if ( SUCCESS == strcasecmp(opCode, "DELETE") )
            delete = 1;
        else
        {
            invalidOpCode = 1;
            rc = ERROR;
            goto rtn;
        }
        printf("\nPASSED OP CODE is %s\n", opCode);
    }

    // KEY
    token = strtok(NULL, ":::");
    if ( token != NULL )
    {
         strcpy(key, token);
         keySet = 1;
         printf("\nKEY: %s\n", key);
    }

    // VALUE 
    if (insert || update)
    {
        token = strtok(NULL, ":::");
        if ( token != NULL )
        {
            strcpy(value, token);
            valueSet = 1;
            printf("\nVALUE: %s\n", value);
        }
    }

    if ( (insert && keySet && valueSet) || (lookup && keySet && !valueSet) || (update && keySet && valueSet) || (delete && keySet && !valueSet) )
    {
        rc = SUCCESS;
        goto rtn;
    }
    else 
    {
        rc = ERROR;
        printf("\nINVALID SYNTAX\n");
    }

  rtn:
    if (invalidOpCode)
        printf("\nINVALID OP CODE. TRY AGAIN!!!");
    return rc;
   
} // End of parseKVClientCmd()

/****************************************************************
 * NAME: createAndSendOpMsg 
 *
 * DESCRIPTION: This function creates message based on op code 
 *              containing key value to be sent to the local 
 *              server
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int createAndSendOpMsg()
{

    int rc = SUCCESS,
        i_rc,
        numOfBytesSent,
        opCodeInt;

    if ( 0 == strcmp (opCode, "INSERT") )
        opCodeInt = INSERT_KV;
    else if ( 0 == strcmp (opCode, "LOOKUP") )
        opCodeInt = LOOKUP_KV;
    else if ( 0 == strcmp (opCode, "DELETE") )
        opCodeInt = DELETE_KV;
    else if ( 0 == strcmp (opCode, "UPDATE") )
        opCodeInt = UPDATE_KV;
    else 
    {
        printf("\nInvalid OP CODE\n");
        rc = ERROR;
        goto rtn;
    }

    msgToSend = NULL;

    switch(opCodeInt)
    {

        case INSERT_KV:

            i_rc = create_message_INSERT(atoi(key), value, &msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create insert message\n");
                rc = ERROR;
                goto rtn;
            }
            i_rc = append_port_ip_to_message(clientPortNo, clientIpAddr, msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create insert message\n");
                rc = ERROR;
                goto rtn;
            }
            numOfBytesSent = sendTCP(atoi(serverPortNo), clientIpAddr, msgToSend);
            if ( SUCCESS == numOfBytesSent )
            {
                printf("\nZERO BYTES SENT\n");
                rc = ERROR;
                goto rtn;
            }
  
        break;

        case LOOKUP_KV:
           
            i_rc = create_message_LOOKUP(atoi(key), &msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create lookup message\n");
                rc = ERROR;
                goto rtn;
            }
            i_rc = append_port_ip_to_message(clientPortNo, clientIpAddr, msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create lookup message\n");
                rc = ERROR;
                goto rtn;
            }
            numOfBytesSent = sendTCP(atoi(serverPortNo), clientIpAddr, msgToSend);
            if ( SUCCESS == numOfBytesSent )
            {
                printf("\nZERO BYTES SENT\n");
                rc = ERROR;
                goto rtn;
            }

        break;

        case UPDATE_KV:
   
            i_rc = create_message_UPDATE(atoi(key), value, &msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create update message\n");
                rc = ERROR;
                goto rtn;
            }
            i_rc = append_port_ip_to_message(clientPortNo, clientIpAddr, msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create update message\n");
                rc = ERROR;
                goto rtn;
            }
            numOfBytesSent = sendTCP(atoi(serverPortNo), clientIpAddr, msgToSend);
            if ( SUCCESS == numOfBytesSent )
            {
                printf("\nZERO BYTES SENT\n");
                rc = ERROR;
                goto rtn;
            }
 
        break;

        case DELETE_KV:

            i_rc = create_message_DELETE(atoi(key), &msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create delete message\n");
                rc = ERROR;
                goto rtn;
            }
            i_rc = append_port_ip_to_message(clientPortNo, clientIpAddr, msgToSend);
            if ( ERROR == i_rc )
            {
                printf("\nUnable to create delete message\n");
                rc = ERROR;
                goto rtn;
            }
            numOfBytesSent = sendTCP(atoi(serverPortNo), clientIpAddr, msgToSend);
            if ( SUCCESS == numOfBytesSent )
            {
                printf("\nZERO BYTES SENT\n");
                rc = ERROR;
                goto rtn;
            }

        break;

        default:

            printf("\nINVALID OP CODE\n");
            rc = ERROR;
            goto rtn;

        break;

    } // End of switch(opCode)

  rtn:
    return rc;

} // End of createAndSendOpMsg()

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

    printf("\n");
    printf("\t\t***************************************\n");
    printf("\t\t***************************************\n");
    printf("\t\tWelcome to the Embedded Daisy KV client\n");
    printf("\t\t***************************************\n");
    printf("\t\t***************************************\n");
    printf("\n");

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
    strcpy(serverPortNo, argv[3]);
    memset(KVclientCmd, '\0', LONG_BUF_SZ);
    strcpy(KVclientCmd, argv[4]);

    strcpy(ipAddress, clientIpAddr);

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


