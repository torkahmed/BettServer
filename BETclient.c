/* Filename: BETclient.c
 * Author: Ahmed Tourk
 * Date: 05.09.2018
 * Description: Client Implementation for BetServer
 */

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "SOCKETwrapper.h"

/*
 * MACROS
 */
#ifdef RAND_MAX
  /* Suppress Redefinition Warning */
  #undef RAND_MAX
#endif
#define RAND_MAX              (0xAA)

/*
 * GLOBAL VARIABLES
 */


/*
 * PRIVATE FUNCTION DECLARATION
 */
bool clientConnectToServer(char *serverIP, int16_t serverPort);

/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */
bool clientConnectToServer(char *serverHumanRIP, int16_t serverPort)
{
    uint32_t serverIP = 0;
    int32_t socketDescriptor;
    mBetServerMessageHeader messageHeader;
    mBetServerMessageAccept messageAccept;
    int8_t nrBytesSent = 0;

    /* Step 1: Validate IP Address Entry */
    if (inet_pton(AF_INET, serverHumanRIP, &serverIP) != 1)
    {
        fprintf(stderr, "[E] Provided Server IPV4 Address is Invalid");
        return false;
    }

    if (!(SW_ConnectToServer(&socketDescriptor, serverIP, serverPort)))
    {
        fprintf(stderr, "[E] Connection to Server Failed");
        return false;
    }

//TODO Connected to Server, send request
    messageHeader.u8Version = PROTOCOL_VERSION;
    messageHeader.u8Length = sizeof(messageHeader);
    messageHeader.u8Type = BETSERVER_OPEN;
    messageHeader.u16ClientID = 0;
    nrBytesSent = send(socketDescriptor, &messageHeader, sizeof(messageHeader), 0);
    if(nrBytesSent != -1)
    {
        fprintf(stderr, "[I] Message sent to server\n");
    }

    recv(socketDescriptor, &messageHeader, sizeof(messageHeader), 0);
    recv(socketDescriptor, &messageAccept, sizeof(messageAccept), 0);
    fprintf(stderr, "[I] MY ID: %d\n", messageHeader.u16ClientID);
    fprintf(stderr, "[I] LB: %x, UB: %x\n", messageAccept.u32BetLowerBounds, messageAccept.u32BetUpperBounds);

}

/*
 * PUBLIC FUNCTIONS
 */

int main(int argc, char const *argv[])
{
    clientConnectToServer("127.0.0.1", 2222);
    return 0;
}
