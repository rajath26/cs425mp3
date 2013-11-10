//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//    FILE NAME: tcp.c
//
//    DECSRIPTION: This is the source file for UDP functionality  
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
#include "../inc/tcp.h"

/*
 * Function declarations
 */

/*****************************************************************
 * NAME: recvTCP
 *
 * DESCRIPTION: This function is wrapper over recv 
 *              
 * PARAMETERS: 
 * (char *) buffer - buffer received to be sent back 
 * (int) length - max length of buffer received 
 * (struct sockaddr_in) hostAddr - struct holding address of 
 *                                 host buffer received from
 *
 * RETURN:
 * (int) bytes received 
 * 
 ****************************************************************/
int recvTCP(char *buffer, int length, struct sockaddr_in hostAddr)
{

    funcEntry(logF, ipAddress, "recvTCP");

    int numOfBytesRec;       // Number of bytes received

    int new_tcp;             // New TCP returne by accept

    socklen_t len;           // Length

    len = sizeof(hostAddr);

    new_tcp = accept(tcp, (struct sockaddr *) &hostAddr, &len);
    if ( new_tcp < SUCCESS )
    {
        strcpy(logMsg, "\nUNABLE TO ACCEPT NEW TCP\n");
        printToLog(logF, ipAddress, logMsg);
        printf("\n%s\n", logMsg);
        numOfBytesRec = 0;
        goto rtn;
    }

    printToLog(logF, ipAddress, "ACCEPT SUCCESSFUL IN RECV TCP");
    printToLog(logF, ipAddress, "I AM HERE WAITING FOR RECVTCP ************************************");

    numOfBytesRec = recv(new_tcp, buffer, length, 0);
    //network_to_host(buffer);

  rtn:
    funcExit(logF, ipAddress, "recvTCP", numOfBytesRec);
    return numOfBytesRec;

} // End of recvTCP()

/*****************************************************************
 * NAME: sendTCP
 *
 * DESCRIPTION: This function is wrapper over send 
 *              
 * PARAMETERS: 
 * (int) portNo - port no
 * (char *) ipAddr - IP 
 * (char *) buffer - buffer to be sent 
 * 
 * RETURN:
 * (int) bytes sent 
 * 
 ****************************************************************/
int sendTCP(int portNo, char * ipAddr, char * buffer)
{

    funcEntry(logF, ipAddress, "sendUDP");

    int numOfBytesSent=0;               // Number of bytes sent 

    struct sockaddr_in hostAddr;        // Address of host to send message

    sprintf(logMsg, "port no : %d ip address : %s", portNo, ipAddr);
    printToLog(logF, ipAddress, logMsg);

    memset(&hostAddr, 0, sizeof(struct sockaddr_in));
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(portNo);
    hostAddr.sin_addr.s_addr = inet_addr(ipAddr);
    memset(&(hostAddr.sin_zero), '\0', 8);

    if ( connect( tcp, (struct sockaddr *) &hostAddr, sizeof(hostAddr) ) < SUCCESS )
    {
        strcpy(logMsg, "Cannot connect to server");
        printToLog(logF, ipAddress, logMsg);
        printf("\n%s\n", logMsg);
        numOfBytesSent = 0;
        goto rtn;
    }
               
    //host_to_network(buffer);
    numOfBytesSent = send(tcp, buffer, strlen(buffer), 0);

rtn:
    funcExit(logF, ipAddress, "sendTCP", numOfBytesSent);
    return numOfBytesSent; 

} // End of sendTCP()

/*
 * End
 */
