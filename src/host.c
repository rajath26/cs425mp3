/////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: host.c
//
//    DECSRIPTION: This is the source file for the leader host 
//                 and the member host
//
//    OPERATING SYSTEM: Linux UNIX only
//    TESTED ON:
//
//    CHANGE ACTIVITY:
//    Date        Who      Description
//    ==========  =======  ===============
//    09-29-2013  Rajath   Initial creation
//    11-07-2013  Rajath   Creating Key-Value store 
//
//****************************************************************************
//////////////////////////////////////////////////////////////////////////////

#include "../inc/host.h"
//#include "logger.c"
#include "udp.c"
#include "message_table.c"

/*
 * Function definitions
 */

/*****************************************************************
 * NAME: CLA_checker 
 *
 * DESCRIPTION: This function is designed to check if command 
 *              line arguments is valid 
 *              
 * PARAMETERS: 
 *            (int) argc - number of command line arguments
 *            (char *) argv - two command line arguments apart 
 *                            from argv[0] namely:
 *                            i) port no
 *                            ii) Ip Address of host
 *                            iii) Host Type 
 *                                 "leader" -> Leader Node
 *                                 "member" -> Member Node
 *                            iv) Host ID
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int CLA_checker(int argc, char *argv[])
{

    funcEntry(logF, ipAddress, "CLA_checker");

    int rc = SUCCESS;        // Return code

    if ( argc != NUM_OF_CL_ARGS )
    {
        printf("\nInvalid usage\n");
        printf("\nUsage information: %s <port_no> <ip_address> <host_type> <host_id>", argv[0]);
        rc = ERROR;
        goto rtn;
    }
    
  rtn:
    funcExit(logF, ipAddress, "CLA_checker", rc);
    return rc;

} // End of CLA_checker()

/*****************************************************************
 * NAME: setUpPorts 
 *
 * DESCRIPTION: This function is designed to create a UDP and 
 *              bind to the port and to create another TCP port
 *              and bind to that port 
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
int setUpPorts(char * portNo, char * ipAddress)
{

    funcEntry(logF, ipAddress, "setUpPorts");
    
    int rc = SUCCESS,        // Return code
        i_rc;                // Temp RC
 
    // Create a UDP socket
    udp = socket(AF_INET, SOCK_DGRAM, 0);
    if ( ERROR == udp )
    {
        printf("\nUnable to open socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("socket");
        printToLog(logF, ipAddress, "UDP socket() failure");
        rc = ERROR;
        goto rtn;
    }
  
    printToLog(logF, ipAddress, "UDP socket() successful");

    // Create a TCP socket 
    tcp = socket(AF_INET, SOCK_STREAM, 0);
    if ( ERROR == tcp )
    {
        printf("\nUnable to open socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("socket");
        printToLog(logF, ipAddres, "TCP socket() failure");
        rc = ERROR;
    }

    printToLog(logF, ipAddress, "TCP socket() successful");

    memset(&hostAddress, 0, sizeof(struct sockaddr_in));
    hostAddress.sin_family = AF_INET;
    hostAddress.sin_port = htons(atoi(portNo)); 
    hostAddress.sin_addr.s_addr = inet_addr(ipAddress);
    memset(&(hostAddress.sin_zero), '\0', 8);

    // Bind the UDP socket
    i_rc = bind(udp, (struct sockaddr *) &hostAddress, sizeof(hostAddress));
    if ( ERROR == i_rc )
    {
        printf("\nUnable to bind UDP socket\n");
        printf("\nError number: %d\n", errno);
        printf("\nExiting.... ... .. . . .\n");
        perror("bind");
        printToLog(logF, ipAddress, "UDP bind() failure");
        rc = ERROR;
        goto rtn;
    }

    printToLog(logF, ipAddress, "UDP bind() successful");

    i_rc = bind(tcp, (struct sockaddr *) &hostAddress, sizeof(hostAddress));
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

    printToLog(logF, ipAddress, "TCP bind() successful");

  rtn:
    funcExit(logF, ipAddress, "setUpPorts", rc);
    return rc;

} // End of setUpPorts()

/*****************************************************************
 * NAME: requestMembershipToLeader 
 *
 * DESCRIPTION: This function is designed to let member host 
 *              request leader node membership to Daisy 
 *              distributed system
 *              
 * PARAMETERS: 
 *            (char *) leaderPort: leader port number
 *            (char *) leaderIp: leader IP Address
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int requestMembershipToLeader(int leaderPort, char *leaderIp)
{

    funcEntry(logF, ipAddress, "requestMembershipToLeader");

    int rc = SUCCESS,                            // Return code 
        i_rc,                                    // Temp RC
        numOfBytesSent;                          // Num of bytes sent

    char joinMessage[LONG_BUF_SZ],                // Buffer
         joinOperation[SMALL_BUF_SZ] = "JOIN$",  // Join prefix
         tableMessage[LONG_BUF_SZ];              // Table msg

    /*
     * Construct join message
     */
    printToLog(logF, ipAddress, "Message to be sent leader node is:");
    i_rc = create_message(tableMessage);
    sprintf(joinMessage, "%s%s", joinOperation, tableMessage);
    printToLog(logF, ipAddress, joinMessage);
    printToLog(logF, ipAddress, "Sending message to leader node");
    numOfBytesSent = sendUDP(leaderPort, leaderIp, joinMessage);
    sprintf(logMsg, "Num of bytes of join msg sent to leader: %d", numOfBytesSent);
    printToLog(logF, ipAddress, logMsg);
    // If number of bytes sent is 0
    if ( SUCCESS == numOfBytesSent)
    {
        rc = ERROR; 
        goto rtn;
    }
    printToLog(logF, ipAddress, "Join message sent successfully");

  rtn:
    funcExit(logF, ipAddress, "requestMembershipToLeader", rc);
    return rc;

} // End of requestMembershipToLeader()

/*****************************************************************
 * NAME: CLI_UI 
 *
 * DESCRIPTION: This function is designed to display CLI UI for 
 *              member hosts
 *              
 * PARAMETERS: NONE
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int CLI_UI()
{

    funcEntry(logF, ipAddress, "CLI_UI");

    int rc = SUCCESS,                    // Return code
        i_rc,                            // Temp RC
        leaderPortNo;                    // Leader port no

    char leaderIpAddress[SMALL_BUF_SZ];  // Buffer to hold leader ip
    
    printf("\n");
    printf("\t\t***********************************************************\n");
    printf("\t\t***********************************************************\n");
    printf("\t\tI am a Member host wanting to join Daisy distributed system\n");
    printf("\t\t***********************************************************\n");
    printf("\t\t***********************************************************\n");
    printf("\n\t\tInput the IP address of the Leader node:\n");
    scanf("%s", leaderIpAddress);
    printf("\n\t\tInput the Port No of the Laeder node:\n");
    scanf("%d", &leaderPortNo);
    sprintf(logMsg, "Trying to join %s at %d", leaderIpAddress, leaderPortNo);
    printToLog(logF, ipAddress, logMsg);
    i_rc = requestMembershipToLeader(leaderPortNo, leaderIpAddress);
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }
  
  rtn:
    funcExit(logF, ipAddress, "CLI_UI", rc);
    return rc;

} // End of CLI_UI()

/*****************************************************************
 * NAME: askLeaderIfRejoinOrNew 
 *
 * DESCRIPTION: This function is executed for leader to determine 
 *              if this is the first call of leader or if leader
 *              had crashed and wants to rejoin 
 *              
 * PARAMETERS: NONE
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int askLeaderIfRejoinOrNew()
{

    funcEntry(logF, ipAddress, "askLeaderIfRejoinOrNew");

    int rc = SUCCESS,                // Return code
        choice,                      // Choice
        memNo;                       // Member host no
 
    char memIP[SMALL_BUF_SZ],        // Member IP
         memPort[SMALL_BUF_SZ];      // Member Port
        

    while(1)
    {
        printf("\n");
        printf("\t\t***************************************\n");
        printf("\t\t***************************************\n");
        printf("\t\tWelcome to the Daisy Distributed System\n");
        printf("\t\t***************************************\n");
        printf("\t\t***************************************\n");
        printf("\nIs this:\n 1)First incarnation of the leader or \n2)Reincarnation of the leader to join back?\n");
        scanf("%d", &choice);

        if ( NEW_INCARNATION == choice )
        {
            goto rtn;
        }
        else if ( REINCARNATION == choice )
        {
            printf("\nInput the IP of atleast one other member in the Daisy distributed system:\n");
            scanf("%s", memIP);
            printf("\nInput the Port No of other member (IP chosen above):\n");
            scanf("%s", memPort);
            printf("\nInput host no of the other member (same as one chosen above):\n");
            scanf("%d", &memNo);
            initialize_table_with_member(memPort, memIP, memNo);
            goto rtn;
        }
        else 
        {
            continue;
        }
    } // End of whie(1)

  rtn:
    funcExit(logF, ipAddress, "askLeaderIfRejoinOrNew", rc);
    return rc;

} // End if askLeaderIfRejoinOrNew()

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
   
    funcEntry(logF, ipAddress, "spawnHelperThreads");

    int rc = SUCCESS,                    // Return code
        i_rc,                            // Temp RC
        threadNum = 0,                   // Thread counter
        *ptr[NUM_OF_THREADS];         // Pointer to thread counter
    
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
            printToLog(logF, ipAddress, "pthread() failure");
            goto rtn;
        }
        printToLog(logF, ipAddress, "pthread() success");
        //free(ptr);
    }

    for ( counter = 0; counter < NUM_OF_THREADS; counter++ )
    {
        pthread_join(threadID[counter], NULL);
    }

  rtn:
    funcExit(logF, ipAddress, "spawnHelperThreads", rc);
    return rc;

} // End of spawnHelperThreads();

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

    sprintf(logMsg, "This is thread with counter: %d and thread ID: %lu", *counter, tid);
    printToLog(logF, ipAddress, logMsg);

    switch(*counter)
    {
        case 0:
        // First thread calls receiver function that does:
        // i) Approve join requests if LEADER
        // ii) Receive heartbeats
        strcat(logMsg, "\texecuting receiverFunc");
        printToLog(logF, ipAddress, logMsg);
        i_rc = receiverFunc(); 
        break;

        case 1:
        // Second thread calls sender function that does:
        // i) Sends heartbeats
        strcat(logMsg, "\texecuting sendFunc");
        printToLog(logF, ipAddress, logMsg);
        i_rc = sendFunc();
        break;

        case 2:
        // Third thread calls heartbeat checker function that:
        // i) checks heartbeat table
        strcat(logMsg, "\texecuting heartBeatCheckerFunc");
        printToLog(logF, ipAddress, logMsg);
        i_rc = heartBeatCheckerFunc();
        break;

	/*case 3: 
	// Fourth thread calls the send key value function that 
	// i) Receives the instruction from the user 
	// ii) Routes the request to the right node
        strcat(logMsg, "\texecuting sendKVFunc");
	i_rc = sendKVFunc();
	break;*/

	case 3:
	// Fourth thread calls receive key value function that 
	// i) Receives operation instructions from send KV thread
	// ii) Calls respective APIs to perform them on local kV store
	strcat(logMsg, "\texecuting receiveKVFunc");
	i_rc = receiveKVFunc();
	break;

	case 4: 
	// Fifth thread calls key value store reorder function that
	// i) Reorders local key value store whenever a node joins 
	//    the distributed system
	// ii) Reorders local key value store wheneve a node leaves 
	//     the distributed system 
	strcat(logMsg, "\texecuting localKVReorderFunc");
	i_rc = localKVReorderFunc();
	break;

        case 5: 
        // Sixth thread calls functtion to print local key value store
        i_rc = printKVStore();
        break;

        default:
        // Can't get here if we do then exit
        printToLog(logF, ipAddress, "default case. An error");
        rc = ERROR;
        goto rtn;
        break;
    } // End of switch

  rtn:
    funcExit(logF, ipAddress, "startKelsa", rc);
    return rc;
} // End of startKelsa()

/****************************************************************
 * NAME: receiverFunc 
 *
 * DESCRIPTION: This is the function that takes care of servicing
 *              the first among the three threads spawned i.e.
 *              the receiver threads which does the following:
 *              i) If I am a LEADER approve join requests from 
 *                 member hosts
 *              ii) Receive heartbeats
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int receiverFunc()
{

    funcEntry(logF, ipAddress, "receiverFunc");

    int rc = SUCCESS,                    // Return code
        numOfBytesRec,                   // Number of bytes received
        i_rc,                            // Temp RC
        op_code;                         // Operation code

    char recMsg[LONG_BUF_SZ],            // Received message
         tokenRecMsg[LONG_BUF_SZ],       // Received message without join op code 
         buffer[SMALL_BUF_SZ];           // Temp buffer

    struct sockaddr_in memberAddress;    // Address of host 
 
    struct hb_entry * recMsgStruct;      // Heart beat table that holds received message

    recMsgStruct = (struct hb_entry *) malloc(4*sizeof(struct hb_entry));

    /*
     * 1) Receive UDP packet
     * 2) Check operation code
     * 3) If JOIN message: 
     *    i) Extract message
     *    ii) Update heartbeat table
     * 4) Else
     *    i) Extract message 
     *    ii) Update heartbeat table
     */

    for(;;)
    {
        /////////
        // Step 1
        /////////
        memset(recMsg, '\0', LONG_BUF_SZ);
        
        // Debug
        printToLog(logF, "recMsg before recvUDP", recMsg);

        numOfBytesRec = recvUDP(recMsg, LONG_BUF_SZ, memberAddress);
        // Check if 0 bytes is received 
        if ( SUCCESS == numOfBytesRec )
        {
             sprintf(logMsg, "Number of bytes received is ZERO = %d", numOfBytesRec);
             printf("\n%s\n", logMsg);
             printToLog(logF, ipAddress, logMsg);
             continue;
        }

        // Debug
        printToLog(logF, "recMsg after recvUDP", recMsg);

        /////////
        // Step 2
        /////////
        i_rc = checkOperationCode(recMsg, &op_code, tokenRecMsg);
        if ( i_rc != SUCCESS ) 
        {
            printToLog(logF, ipAddress, "Unable to retrieve opcode");
            continue;
        }
        /////////
        // Step 3
        /////////
        if ( JOIN_OP_CODE == op_code )
        {
            sprintf(logMsg, "JOIN msg from %s", inet_ntop(AF_INET, &memberAddress.sin_addr, buffer, sizeof(buffer)));
            printToLog(logF, ipAddress, logMsg);
            ///////////
            // Step 3i
            ///////////
            
            // Debug. uncomment if req
            printToLog(logF, ipAddress, "\nBefore clear_temp_entry_table\n"); 

            clear_temp_entry_table(recMsgStruct);

            printToLog(logF, ipAddress, "\nAfter c_t_e_t\n");

            printToLog(logF, ipAddress, "\nbefore extract_msg\n");

            sprintf(logMsg, "Token received message before e_m: %s", tokenRecMsg);
            printToLog(logF, ipAddress, logMsg);

            recMsgStruct = extract_message(tokenRecMsg);

            printToLog(logF, ipAddress, "\nafter e_m\n");

            //printToLog(logF, ipAddress, "\nToken Received Message: %s", tokenRecMsg);

            sprintf(logMsg, "Token received message: %s", tokenRecMsg);
            printToLog(logF, ipAddress, logMsg);

            if ( NULL == recMsgStruct )
            {
                printToLog(logF, ipAddress, "Unable to extract message");
                continue;
            }
            ////////////
            // Step 3ii
            ////////////
            i_rc = update_table(recMsgStruct);
            if ( i_rc != SUCCESS )
            {
                 printToLog(logF, ipAddress, "Unable to update heart beat table");
                 continue;
            }

        } // End of if ( JOIN_OP_CODE == op_code )
        /////////
        // Step 4
        /////////
        else
        {
            //////////
            // Step 4i
            //////////
            clear_temp_entry_table(recMsgStruct);
            recMsgStruct = extract_message(recMsg);
            if ( NULL == recMsgStruct )
            {
                printToLog(logF, ipAddress, "Unable to extract message");
                continue;
            }
            ///////////
            // Step 4ii
            ///////////
            i_rc = update_table(recMsgStruct);
            if ( i_rc != SUCCESS )
            {
                 printToLog(logF, ipAddress, "Unable to update heart beat table");
                 continue;
            }
        } // End of else
    } // End of for(;;)

  rtn:
    funcExit(logF, ipAddress, "receiverFunc", rc);
    return rc;

} // End of receiverFunc()

/****************************************************************
 * NAME: checkOperationCode 
 *
 * DESCRIPTION: This is the function checks the passed in message
 *              and determines if it is a JOIN message or not
 *              
 * PARAMETERS: 
 * (char *) recMsg - received message
 * (int) op_code - pass by reference back to calling function
 *                 JOIN_OP_CODE if JOIN message else 
 *                 RECEIVE_HB_OP_CODE
 * (char *) tokenRecMsg - pass by reference back to calling 
 *                        function if JOIN_OP_CODE. Message with
 *                        JOIN OP CODE removed
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int checkOperationCode(char *recMsg, int *op_code, char *tokenRecMsg)
{

    funcEntry(logF, ipAddress, "checkOperationCode");
    
    int rc = SUCCESS;              // Return code
 
    char *token;                   // Token

    // Debug
    printToLog(logF, "recMsg in checkOpCode", recMsg); 

    token = strtok(recMsg, "$");

    // Debug
    printToLog(logF, "token *****", token);

    if ( (NULL != token) && (SUCCESS == strcmp(token, "JOIN")) ) 
    {
        printToLog(logF, ipAddress, "JOIN Op");
        *op_code = JOIN_OP_CODE;
        // Debug
        printToLog(logF, "token *****", token); 
        token = strtok(NULL, "$"); 
        // Debug
        printToLog(logF, "token *****", token);
        strcpy(tokenRecMsg, token);
        printToLog(logF, ipAddress, tokenRecMsg);
    }
    else
    {
        *op_code = RECEIVE_HB_OP_CODE;
        printToLog(logF, ipAddress, "RECEIVE HB Op");
        printToLog(logF, ipAddress, recMsg);
    }
    
  rtn:
    funcExit(logF, ipAddress, "checkOperationCode", rc);
    return rc;

} // End of checkOperationCode()

/****************************************************************
 * NAME: sendFunc 
 *
 * DESCRIPTION: This is the function that takes care of sending
 *              heartbeats 
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int sendFunc()
{

    funcEntry(logF, ipAddress, "sendFunc");

    int rc = SUCCESS,                      // Return code
        num_of_hosts_chosen,               // Number of hosts chosen 
        i_rc,                              // Temp RC
        numOfBytesSent,                    // Number of bytes sent
        portNo;                            // Port no

    register int counter;                  // Counter

    char msgToSend[LONG_BUF_SZ],           // Message to be sent
         ipAddr[SMALL_BUF_SZ],             // IP Address buffer
         portNoChar[SMALL_BUF_SZ];         // Port no

    struct two_hosts hosts[GOSSIP_HOSTS],  // An array of two_hosts
           *ptr;                           // Pointer to above
 
    ptr = hosts;

    while(1)
    {

        memset(msgToSend, '\0', LONG_BUF_SZ);

        // Debug
        printToLog(logF, "SENDOct3", msgToSend);

        initialize_two_hosts(ptr);
        num_of_hosts_chosen = choose_n_hosts(ptr, GOSSIP_HOSTS);

        sprintf(logMsg, "Number of hosts chosen to gossip: %d", num_of_hosts_chosen);
        printToLog(logF, ipAddress, logMsg);

        for ( counter = 0; counter < num_of_hosts_chosen; counter++ )
        {
            printToLog(logF, "PORT NO*****", hb_table[hosts[counter].host_id].port);
            strcpy(portNoChar, hb_table[hosts[counter].host_id].port);
            portNo = atoi(portNoChar);
            strcpy(ipAddr, hb_table[hosts[counter].host_id].IP);
            printToLog(logF, "IP ADDR*****", hb_table[hosts[counter].host_id].IP);
            // create message
            i_rc = create_message(msgToSend);
            if ( SUCCESS != i_rc )
            {
                printToLog(logF, ipAddress, "Unable to create message");
                continue;
            }

            // Debug
            printToLog(logF, "SENDOct3 after create_message", msgToSend);

            // Send UDP packets
            numOfBytesSent = sendUDP(portNo, ipAddr, msgToSend);
            // check if 0 bytes is sent
            if ( SUCCESS == numOfBytesSent )
            {
                printToLog(logF, ipAddress, "ZERO bytes sent");
                continue;
            }
            
            memset(msgToSend, '\0', LONG_BUF_SZ);
        } // End of for ( counter = 0; counter < num_of_hosts_chosen; counter++ )
        sleep(1);
    } // End of while 
    
  rtn:
    funcExit(logF, ipAddress, "sendFunc", rc);
    return rc;

} // End of sendFunc() 

/****************************************************************
 * NAME: heartBeatChecker 
 *
 * DESCRIPTION: This is the function that takes care of checking 
 *              heartbeats
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int heartBeatCheckerFunc()
{

    funcEntry(logF, ipAddress, "heartBeatCheckerFunc");

    int rc = SUCCESS;        // Return code

    while(1)
    {
        sleep(HEART_BEAT_UPDATE_SEC);
        update_my_heartbeat();
        check_table_for_failed_hosts();
        //print_table(hb_table);
    }

  rtn:
    funcExit(logF, ipAddress, "heartBeatChecker", rc);
    return rc;

} // End of heartBeatChecker()

/****************************************************************
 * NAME: leaveSystem 
 *
 * DESCRIPTION: This is the function that provides 
 *              voluntary leave  
 *              
 * PARAMETERS: NONE 
 *
 * RETURN: VOID
 * 
 ****************************************************************/
void leaveSystem(int signNum)
{

    funcEntry(logF, "Leaving Daisy Distributed System", "leaveSystem");

    printToLog(logF, ipAddress, "Preparing the node to leave the Daisy Distributed System");

    i_rc = preNodeForSystemLeave()
    if ( ERROR == i_rc )
    {
         sprintf(logMsg, "Unable to prepare the node with IP address %s and host no %d", ipAddress, host_no);
         printToLog(logF, ipAddress, logMsg);
         printf("\n%s\n", logMsg);
         printToLog(logF, ipAddress, "Proceeding to close to the UDP and TCP ports after receiving an error");
    }  
    
    close(udp);
    close(tcp);

    funcExit(logF, "Leaving Daisy Distributed System", "leaveSystem", 0);

} // End of leaveSystem()

/****************************************************************
 * NAME: initialize_key_value_store 
 *
 * DESCRIPTION: This is the function that initializes the local
 *              key-value store
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int initialize_key_value_store()
{

    funcEntry(logF, ipAddress, "initialize_key_value_store");

    int rc = SUCCESS,        // Return code
        i_rc;                // Temp RC

    // Create the hash table that stores the local key value store
    i_rc = create_hash_table()
    if ( i_rc != SUCCESS )
    {
         sprintf(logMsg, "\nUnable to create local key value store at IP address %s and host no %d\n", ipAddress, host_no);
	 printf("%s", logMsg);
	 printToLog(logF, ipAddress, logMsg);
	 rc = ERROR;
	 goto rtn;
    }

    printToLog(logF, ipAddress, "Created local key value store successfully");

  rtn:
    funcExit(logF, ipAddress, "initialize_key_value_store", rc);
    return rc;
    
} // End of initialize_key_value_store()

/****************************************************************
 * NAME: sendKVFunc 
 *
 * DESCRIPTION: THis is the function that is responsible for 
 *              i) accepting the action from the user on the key
 *              value store 
 *              ii) construct KV functionality op code
 *              iii) determine the host to route the request
 *              iv) create the message with KV op code
 *              v) send the message to the node
 *
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
/*
int sendKVFunc()
{

    funcEntry(logF, ipAddress, "sendKVFunc");

    int rc = SUCCESS,           // Return code
        i_rc,                   // Temp RC 
        chosenKVFunc;           // KV functionality chosen

    for (;;)
    {
        /////////
        // Step i
        /////////
        // Accept the action from the user on the local key value 
        // store
        chosenKVFunc = displayKVFunctionalities();
        if ( (chosenKVFunc != INSERT_KV) || (chosenKVFunc != LOOKUP_KV) || (chosenKVFunc != UPDATE_KV) || (chosenKVFunc != DELETE_KV) )
        {
             printToLog(logF, ipAddress, "Improper functionality chosen");
             continue;
	}

	//////////
	// Step ii
	//////////
	// Construct the KV functionality op code

    }

  rtn:
    funcExit(logF, ipAddress, "sendKVFunc", rc);
    return rc;

} // End of sendKVFunc()
*/

/****************************************************************
 * NAME: displayKVFunctionalities 
 *
 * DESCRIPTION: This is the function that is responsible for 
 *              displaying the key value store functionalities 
 *              and accepting the input from the user 
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) INSERT_KV if insert functionality chosen  
 *       LOOKUP_KV if lookup functionality chosen
 *       UPDATE_KV if update functionality chosen
 *       DELETE_KV if delete functionality chosen
 *       PRINT_KV  if print  functionality chosen
 * 
 ****************************************************************/
 /*
int displayKVFunctionalities()
{

    funcEntry(logF, ipAddress, "displayKVFunctionalities");

    int chosenKVFunc;            // Chosen KV functionality

    printf("\n");
    printf("\t\t**************************\n");
    printf("\t\t**************************\n");
    printf("\t\tWhat would you like to do?\n");
    printf("\t\t**************************\n");
    printf("\t\t**************************\n");
 
    printf("\n");
    printf("\t\t1) INSERT\n");
    printf("\t\t2) UPDATE\n");
    printf("\t\t3) DELETE\n");
    printf

    
  rtn:
    funcExit(logF, ipAddress, "displayKVFuncionalities", chosenKVFunc);
    return chosenKVFunc;

} // End of displayKVFunctionalities()
*/

/****************************************************************
 * NAME: receiveKVFunc 
 *
 * DESCRIPTION: This is the function that is responsible for 
 *              i) Receiving request from either the local client
 *                 or a peer server
 *              ii) Extract received and original requestor IP and 
 *                  port and save it
 *              iii) Extract received message 
 *              iv) Determine where to route the request
 *              v) if routing returns local
 *                 then  
 *                     based on op code 
 *                     call respective API
 *                     send result back to the requestor
 *                 else
 *                     send the result to another peer node
 *                 fi
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int receiveKVFunc()
{

    funcEntry(logF, ipAddress, "receiveKVFunc");

    int rc = SUCCESS,                     // Return code
        numOfBytesRec,                    // Number of bytes received
	numOfBytesSent,                   // Number of bytes sent
	i_rc,                             // Temp RC
        hash_index,                       // Hash index to host
        portPN;                           // Hashed peer node port

    char recMsg[LONG_BUF_SZ],             // Received message 
         lookupValue[LONG_BUF_SZ],        // Lookup value buffer
	 retMsg[LONG_BUF_SZ],             // Message to be returned to original requestor
         ipAddrPN[SMALL_BUF_SZ];          // IP Address of hashed peer node

    struct sockaddr_in receivedFromAddr;  // Predecessor address  

    struct op_code *temp = NULL;          // KV OPCODE

    for(;;)
    {
         // Set all to NULL
	 memset(recMsg, '\0', LONG_BUF_SZ);
	 memset(lookupValue, '\0', LONG_BUF_SZ);
	 memset(retMsg, '\0', LONG_BUF_SZ);
         memset(ipAddrPN, '\0', SMALL_BUF_SZ);
	 temp = NULL;
         memset(&receivedFromAddr, 0, sizeof(struct sockadddr_in));
	 numOfBytesRec = 0;
         numOfBytesSent = 0;

	 // Debug
	 printToLog(logF, "recMsg before recvUDP", recMsg);

         /////////
	 // Step i
	 /////////
	 // Receive TCP message 
	 numOfBytesRec = recvTCP(recMsg, LONG_BUF_SZ, &receivedFromAddr);
	 // Check if 0 bytes is received 
	 if ( SUCCESS == numOfBytesRec )
	 {
             sprintf(logMsg, "Number of bytes received is ZERO = %d", numOfBytesRec);
	     printf("\n%s\n", logMsg);
	     printToLog(logF, ipAddress, logMsg);
	     continue;
	 }

	 // Debug
	 printToLog(logF, "recMsg after recvTCP", recMsg);
         sprintf(logMsg, "number of bytes received %d", numOfBytesRec);
         printToLog(logF, ipAddress, logMsg); 

	 //////////
	 // Step ii
	 //////////
         // Extract and store predecessor and original requestor 
	 // information
         // Original requestor information will be stored in 
	 // temp op_code members port and ipAddr 
	 // predecessor address will be in receivedFromAddr
         sprintf(logMsg, "Original Requestor Port No: %d, IP Address: %s", temp->port, temp->ipAddr);
         printToLog(logF, ipAddress, logMsg);

	 ///////////
	 // Step iii
	 ///////////
	 // Extract received message
	 i_rc = extract_message_op(recMsg, &temp);
	 if ( ERROR == i_rc )
	 {
	     sprintf(logMsg, "Unable to extract received message. Return code of extract_message_op = %d", i_rc);
	     printToLog(logF, ipAddress, logMsg);
	     continue;
	 }

         printToLog(logF, "successfully extracted message", recMsg);
         sprintf(logMsg, "opcode: %d, key: %d, value: %s", temp->opcode, temp->key, *(temp->value));
         printToLog(logF, ipAddress, logMsg);

	 //////////
	 // Step iv
	 //////////
	 // Determine where to route the request
	 // i_rc is the hash index of the host in this case
	 hash_index = choose_host_hb_index(temp->key);
	 if ( ERROR == hash_index )
	 {
              sprintf(logMsg, "Unable to choose host to route the request. Return code of choose_host_hb_index() = %d", i_rc);
	      printToLog(logF, ipAddress, logMsg);
	      continue;
	 }

         sprintf(logMsg, "Hash index : %d", hash_index); 
         printToLog(logF, ipAddress, logMsg);

	 /////////
	 // Step v
	 /////////
	 // If routing returns local index implies we have to perform
	 // the requested operation on our local key value store 
	 // else just send the original message to the host returned 
	 // by choose_host_hb_index

         ///////////
         // If LOCAL 
         ///////////
	 if ( hash_index == my_hash_index )
	 {

             printToLog(logF, ipAddress, "Local route");

             // Do the operation on the local key value store 
	     // based on the KV opcode
	     switch( temp->opcode )
	     {

	         case INSERT_KV:
		     // Insert the KV pair in to the KV store
                     i_rc = insert_key_value_into_store(temp);
		     // If error send an error message to the original
		     // requestor
		     if ( ERROR == i_rc )
		     {
                          sprintf(logMsg, "There was an ERROR while INSERTING %d = %s KV pair into the local KV store", temp->key, *(temp->value));
			  printToLog(logF, ipAddress, logMsg);
			  i_rc = create_message_ERROR(temp, retMsg);
			  if ( ERROR == i_rc )
			  {
			      printToLog(logF, ipAddress, "Error while creating ERROR_MESSAGE");
			      continue;
			  }
			  numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
			  if ( SUCCESS == numOfBytesSent )
			  {
                              printToLog(logF, ipAddress, "ZERO BYTES SENT");
			      continue;
			  }
		     }
                     // If successful send a success message to the original 
                     // requestor
		     else 
		     {
                         sprintf(logMsg, "KV pair %d = %s SUCCESSFULLY INSERTED", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
			 i_rc = create_message_INSERT_RESULT_SUCCESS(temp->key, retMsg);
			 if ( ERROR == i_rc )
			 {
			     printToLog(logF, ipAddress, "Error while creating INSERT_RESULT_SUCCESS_MESSAGE");
			     continue;
			 }
			 numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
	                 if ( SUCCESS == numOfBytesSent )
		         {
			     printToLog(logF, ipAddress, "ZERO BYTES SENT");
			     continue;
			 }
		     }
                 break;

		 case DELETE_KV:
                     // Delete the KV pair in to the KV store
                     i_rc = delete_key_value_store_from_store(temp->key);
                     // If error send an error message to the original 
                     // requestor
		     if ( ERROR == i_rc )
		     {
		         sprintf(logMsg, "There was an ERROR while DELETING %d = %s KV pair into the local KV store", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
                         i_rc = create_message_ERROR(temp, retMsg);
                         if ( ERROR == i_rc )
                         {
                             printToLog(logF, ipAddress, "Error while creating ERROR_MESSAGE");
                             continue;
                         }
                         numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
                         if ( SUCCESS == numOfBytesSent )
                         {
                             printToLog(logF, ipAddress, "ZERO BYTES SENT");
                             continue;
                         }
		     }
                     // If successful send a success message to the original 
                     // requestor
		     else
		     {
		         sprintf(logMsg, "KV pair %d = %s SUCCESSFULLY DELETED", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
                         i_rc = create_message_DELETE_RESULT_SUCCESS(temp->key, retMsg);
                         if ( ERROR == i_rc )
                         {
                             printToLog(logF, ipAddress, "Error while creating DELETE_RESULT_SUCCESS_MESSAGE");
                             continue;
                         }
                         numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
                         if ( SUCCESS == numOfBytesSent )
                         {
                             printToLog(logF, ipAddress, "ZERO BYTES SENT");
                             continue;
                         }
		     }
		 break;

		 case UPDATE_KV:
                     // Update KV in to the KV store
		     i_rc = update_key_value_in_store(temp);
                     // if error send an error message to the original
                     // requestor
		     if ( ERROR == i_rc )
		     {
                         sprintf(logMsg, "There was an ERROR while UPDATING %d = %s KV pair into the local KV store", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
                         i_rc = create_message_ERROR(temp, retMsg);
                         if ( ERROR == i_rc )
                         {
                             printToLog(logF, ipAddress, "Error while creating ERROR_MESSAGE");
                             continue;
                         }
                         numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
                         if ( SUCCESS == numOfBytesSent )
                         {
                             printToLog(logF, ipAddress, "ZERO BYTES SENT");
                             continue;
                         }
		     }
		     else 
		     {
                         sprintf(logMsg, "KV pair %d = %s SUCCESSFULLY UPDATED", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
                         i_rc = create_message_UPDATE_RESULT_SUCCESS(temp->key, retMsg);
                         if ( ERROR == i_rc )
                         {
                             printToLog(logF, ipAddress, "Error while creating UPDATE_RESULT_SUCCESS_MESSAGE");
                             continue;
                         }
                         numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
                         if ( SUCCESS == numOfBytesSent )
                         {
                             printToLog(logF, ipAddress, "ZERO BYTES SENT");
                             continue;
                         }
		     }
		 break;

		 case LOOKUP_KV:
                     // Lookup on the local key value store
	             lookupValue = lookup_store_for_key(temp->key);
                     // If an error send an error message to the original
                     // requestor
                     if ( ERROR == i_rc )
		     {
		         sprintf(logMsg, "There was an ERROR during LOOKUP of %d = %s KV pair in the local KV store", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
                         i_rc = create_message_ERROR(temp, retMsg);
                         if ( ERROR == i_rc )
                         {
                             printToLog(logF, ipAddress, "Error while creating ERROR_MESSAGE");
                             continue;
                         }
                         numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
                         if ( SUCCESS == numOfBytesSent )
                         {
                             printToLog(logF, ipAddress, "ZERO BYTES SENT");
                             continue;
                         }
		     }
                     // If successful send a success message to the orig
                     // requestor
		     else
		     {
		         sprintf(logMsg, "KV pair %d = %s SUCCESSFUL LOOKUP", temp->key, *(temp->value));
			 printToLog(logF, ipAddress, logMsg);
                         i_rc = create_message_LOOKUP_RESULT(temp->key, *(temp->value), retMsg);
                         if ( ERROR == i_rc )
                         {
                             printToLog(logF, ipAddress, "Error while creating UPDATE_RESULT_SUCCESS_MESSAGE");
                             continue;
                         }
                         numOfBytesSent = sendTCP(temp->port, temp->ipAddr, retMsg);
                         if ( SUCCESS == numOfBytesSent )
                         {
                             printToLog(logF, ipAddress, "ZERO BYTES SENT");
                             continue;
                         }
		     }
		 break;

		 case LOOKUP_RESULT:
                     // Nothing here as of now 
		 break;

		 case INSERT_RESULT:
                     // Nothing here as of now 
		 break;

		 case DELETE_RESULT:
                     // Nothing here as of now 
		 break;

		 case UPDATE_RESULT:
                     // Nothing here as of now 
		 break;

                 default:
		     // We should never ever be here 
		     sprintf(logMsg, "Invalid KV OP code received so just continue along");
		     printToLog(logF, ipAddress, logMsg);
		     continue;
		 break;

	     } // End of switch( temp->opcode )

	 } // End of if ( hash_index == my_hash_index )

         //////////////////
         // IF PEER ROUTING
         //////////////////
	 else 
	 {
         
             printToLog(logF, ipAddress, "Peer Node routing");

             // Send the received message to the hashed peer node
             numOfBytesSent = sendTCP(portPN, ipAddrPN, recMsg);
             if ( SUCCESS == numOfBytesSent )
             {
                 printToLog(logF, ipAddress, "ZERO BYTES SENT");
                 continue;
             }

	 } // End of else of if ( hash_index == my_hash_index )

    } // End of for(;;)

  rtn:
    funcExit(logF, ipAddress, "receiveKCFunc", rc);
    return rc;

} // End of receiveKVFunc()

/****************************************************************
 * NAME: localKVReoderFunc 
 *
 * DESCRIPTION: This is the function that is responsible for 
 *              i) Reordering the local KV store whenever the
 *                 trigger for reoder is set
 *              ii) The trigger for reorder is set when a node 
 *                  joins or leaves the Daisy Distributed 
 *                  system
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int localKVReorderFunc()
{

    funcEntry(logF, ipAddress, "localKVReorderFunc");

    int rc = SUCCESS;

    while(1)
    {

        sleep(REORDER_CHECK_TIME_PERIOD);

        printToLog(logF, ipAddress, "I am in localKVReorderFunc");
        sprintf(logMsg, "Reorder trigger value: %d", reOrderTrigger);
        printToLog(logF, ipAddress, logMsg);

        if (reOrderTrigger) 
        {
            i_rc = reorderKVStore();
            if ( ERROR == i_rc )
            {
                printToLog(logF, ipAddress, "error while reordering local KV store");
            }
            else 
            {
                printToLog(logF, ipAddress, "SUCCESSFULLY REORDERED LOCAL KV STORE");
            }
        }

    } // End of while(1)

  rtn:
    funcExit(logF, ipAddress, "localKVReorderFunc", rc);
    return rc;

} // End of localKVReorderFunc

/****************************************************************
 * NAME: printKVStore 
 *
 * DESCRIPTION: This is the function that is responsible for 
 *              printing local KV store
 *              
 * PARAMETERS: NONE 
 *
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int printKVStore()
{

    funcEntry(logF, ipAddress, "printKVStore");

    int rc = SUCCESS,         // Return code
        i_rc;                 // Temp RC

    char input[SMALL_BUF_SZ]; // Input

    printf("\n");
    printf("\t\t***************************************\n");
    printf("\t\t***************************************\n");
    printf("\t\tWelcome to the Embedded Daisy KV store \n");
    printf("\t\t***************************************\n");
    printf("\t\t***************************************\n");
    for (;;)
    {
        printf("\t\tI am the bot to let you print the local KV store\n");
        printf("\t\tHit \"PRINT\" when you want to print the local key value store\n");
        scanf("%s", input);
        if ( SUCCESS == strcmp(input, "PRINT");
           printLocalKVStore();
        else 
           continue;
    }

  rtn:
    funcExit(logF, ipAddress, "printKVStore");
    return rc;

} // End of printKVStore()

/*
 * Main function
 */

/*****************************************************************
 * NAME: main 
 *
 * DESCRIPTION: Main function of the leader host i.e. the contact 
 *              host that approves other hosts to join the
 *              network and the member host. This binary is 
 *              invoked via a start up script. The parameters are
 *              port no and ip address and node type.
 *              
 * PARAMETERS: 
 *            (int) argc - number of command line arguments
 *            (char *) argv - two command line arguments apart 
 *                            from argv[0] namely:
 *                            i) Port No
 *                            ii) Ip Address of host
 *                            iii) Host Type 
 *                                 "leader" -> Leader Node
 *                                 "member" -> Member Node
 *                            iv) Host ID
 * 
 * RETURN:
 * (int) ZERO if success
 *       ERROR otherwise
 * 
 ****************************************************************/
int main(int argc, char *argv[])
{

    int rc = SUCCESS,              // Return code
        i_rc;                            // Intermittent return code
        
    char leaderIpAddress[SMALL_BUF_SZ],  // Buffer to hold leader ip
         leaderPortNo[SMALL_BUF_SZ];     // Buffer to hold leader port no

    /*
     * Init log file 
     */
    i_rc = logFileCreate(LOG_FILE_LOCATION);
    if ( i_rc != SUCCESS )
    {
        printf("\nLog file won't be created. There was an error\n");
        rc = ERROR;
        goto rtn;
    }

    funcEntry(logF, "I am starting", "host::main");

    /*
     * Command line arguments check
     */
    i_rc = CLA_checker(argc, argv);
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }
       
    /*
     * Copy ip address and port no to local buffer
     */
    memset(ipAddress, '\0', SMALL_BUF_SZ);
    sprintf(ipAddress, "%s", argv[2]);
    memset(portNo, '\0', SMALL_BUF_SZ);
    sprintf(portNo, "%s", argv[1]);
    host_no = atoi(argv[4]);

    /*
     * Init local host heart beat table
     */
    initialize_table(portNo, ipAddress, host_no);
    printToLog(logF, ipAddress, "Initialized my table");

    /*
     * Init the local Key Value store i.e. 
     * the hash table 
     */
    i_rc = initialize_local_key_value_store();
    if ( i_rc != IERR_OK )
    {
         sprintf(logMsg, "Unable to initialize local key value store at IP Address %s and host no %d", ipAddress, host_no);
         printf("\n%s\n", logMsg);
         printToLog(logF, ipAddress, logMsg);
         rc = ERROR;
         goto rtn;
    }
         
    /* 
     * Get the node type based on third argument. By default it
     * is always member node.
     */
    if ( SUCCESS == strcmp(argv[3], LEADER_STRING) )
    {
        isLeader = true;
        printToLog(logF, ipAddress, "I am the leader node");
    }
    else 
    {
        printToLog(logF, ipAddress, "I am a member node");
    }

    /* 
     * Set up UDP & TCP
     */
    i_rc = setUpPorts(portNo, ipAddress); 
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        printToLog(logF, ipAddress, "UDP and TCP setup failure");
        goto rtn;
    }

    // Log current status 
    printToLog(logF, ipAddress, "UDP and TCP setup successfully");

    /*
     * If current host is a LEADER then log that this host has
     * joined the distributed system
     */
    if ( isLeader )
    {
        printToLog(logF, ipAddress, "I, THE LEADER have joined the Daisy Distributed System");
    }

    /*
     * Display the CLI UI if this host is a MEMBER host which 
     * asks member if he wants to send join message to leader node
     * and calls the function requestMembershipToLeader() which
     * does the job
     */
    if ( !isLeader )
    {
        i_rc = CLI_UI();
        if ( i_rc != SUCCESS )
        {
            rc = ERROR;
            goto rtn;
        }
    }
    /* 
     * If leader ask if this a new incarnation or a 
     * reincarnation
     */
    else
    {
        i_rc = askLeaderIfRejoinOrNew();
        if ( i_rc != SUCCESS )
        {
            rc = ERROR;
            goto rtn;
        }
    }

    /* 
     * Set up infrastructure for node to leave
     * voluntarily
     */
    signal(SIGABRT, leaveSystem);
    if ( errno != SUCCESS )
    {
        printf("SIGINT set error %d \n", errno);
    }

    /*
     * Spawn the helper threads
     */
    i_rc = spawnHelperThreads();
    if ( i_rc != SUCCESS )
    {
        rc = ERROR;
        goto rtn;
    }


  rtn:
    funcExit(logF, ipAddress, "Host::main", rc);

    /*
     * Close the log
     */ 
    if ( logF != NULL )
    {
        logFileClose(logF);
    }

    return rc;

} // End of main
