/* Filename: BETclient.c
 * Author: Ahmed Tourk
 * Date: 05.09.2018
 * Description: Client Implementation for BetServer
 */

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "SOCKETwrapper.h"

/*
 * MACROS
 */

/*
 * GLOBAL VARIABLES
 */


/*
 * PRIVATE FUNCTION DECLARATION
 */
uint32_t getRandomNumber(uint32_t min, uint32_t max, uint16_t clientID);
bool clientConnectToServer(char *serverIP, int16_t serverPort);

/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */
uint32_t getRandomNumber(uint32_t min, uint32_t max, uint16_t clientID)
{
    //TODO: USE RNG used in ID
    double factor = clientID & 0x00FF;
    factor /= 0xFF;
    fprintf(stderr, "[D] Random Factor: %f\n", factor);
    return (min + ((max-min) * factor));
}

bool clientConnectToServer(char *serverHumanRIP, int16_t serverPort)
{
    uint32_t serverIP = 0;
    int32_t socketDescriptor;
    int8_t nrBytesSent = 0;
    mBetServerMessageHeader messageHeader;
    mBetServerMessageAccept messageAccept;
    mBetServerMessageBet messageBet;

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

    messageBet.u32BettingNumber = getRandomNumber(messageAccept.u32BetLowerBounds, messageAccept.u32BetUpperBounds, messageHeader.u16ClientID);

    fprintf(stderr, "[I] Betting Number: %x\n", messageBet.u32BettingNumber);
    
    return true;
}

/*
 * PUBLIC FUNCTIONS
 */

int main(int argc, char const *argv[])
{
    clientConnectToServer("127.0.0.1", 2222);
    return 0;
}
