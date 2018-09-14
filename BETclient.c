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
#include "BETclient.h"

/*
 * MACROS
 */

/*
 * GLOBAL VARIABLES
 */


/*
 * PRIVATE FUNCTION DECLARATION
 */
#if (USER_INPUT_BETNUM == 1)
uint32_t getBettingNumberFromUser(uint32_t min, uint32_t max);
#else
uint32_t getRandomNumber(uint32_t min, uint32_t max, uint16_t clientID);
#endif
bool clientConnectToServer(char *serverIP, int16_t serverPort);


/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */

#if (USER_INPUT_BETNUM == 1)

/*
 Function: getBettingNumberFromUser
 Description: Take Betting Number from User Input
 */
uint32_t getBettingNumberFromUser(uint32_t min, uint32_t max)
{
    uint32_t u32UserInput = 0;
    do
    {
        fprintf(stdout, "[U] Please Input a Number between %x and %x ==> ", min, max);
        fflush(stdout);
        fscanf(stdin, "%x", &u32UserInput);
        fprintf(stdout, "\n");
        fflush(stdout);
        if ((u32UserInput > max) || (u32UserInput < min))
        {
            fprintf(stdout, "[E] Wrong Entry, Please try again\n");
        }
    } while ((u32UserInput > max) || (u32UserInput < min));
}

#else
/*
 Function: getRandomNumber
 Description: Randomize a number between min and max using the last byte in client ID
 */
uint32_t getRandomNumber(uint32_t min, uint32_t max, uint16_t clientID)
{
    /* RNG Used in ID */
    double factor = clientID & 0x00FF;
    factor /= 0xFF;
    fprintf(stderr, "[D] Random Factor: %f\n", factor);
    return (min + ((max-min) * factor));
}
#endif


bool clientConnectToServer(char *serverHumanRIP, int16_t serverPort)
{
    //TODO: Implement Protection on all send/recv
    uint32_t serverIP = 0;
    int32_t socketDescriptor;
    int8_t nrBytesSent = 0;
    mBetServerMessageHeader messageHeader;
    mBetServerMessageAccept messageAccept;
    mBetServerMessageBet messageBet;
    mBetServerMessageResult messageResult;

    /* Step 1: Validate IP Address Entry */
    if (inet_pton(AF_INET, serverHumanRIP, &serverIP) != 1)
    {
        fprintf(stderr, "[E] Provided Server IPV4 Address is Invalid");
        return false;
    }

    /* Step 2: Connect to Server */
    if (!(SW_ConnectToServer(&socketDescriptor, serverIP, serverPort)))
    {
        fprintf(stderr, "[E] Connection to Server Failed");
        return false;
    }

    /* Step 3: Send Open Message */
    messageHeader.u8Version = PROTOCOL_VERSION;
    messageHeader.u8Length = sizeof(messageHeader);
    messageHeader.u8Type = BETSERVER_OPEN;
    messageHeader.u16ClientID = 0;
    nrBytesSent = send(socketDescriptor, &messageHeader, sizeof(messageHeader), 0);
    if(nrBytesSent != -1)
    {
        fprintf(stderr, "[I] Open Message Sent Successfully\n");
    }

    /* Step 4: Receive Accept Message */
    recv(socketDescriptor, &messageHeader, sizeof(messageHeader), 0);
    recv(socketDescriptor, &messageAccept, sizeof(messageAccept), 0);
    fprintf(stderr, "[I] MY ID: %d\n", messageHeader.u16ClientID);
    fprintf(stderr, "[I] LB: %x, UB: %x\n", messageAccept.u32BetLowerBounds, messageAccept.u32BetUpperBounds);

    /* Step 5: Send Bet Message */
    messageHeader.u8Length = sizeof(messageHeader) + sizeof(messageBet);
    messageHeader.u8Type = BETSERVER_BET;
#if (USER_INPUT_BETNUM == 1)
    messageBet.u32BettingNumber = getBettingNumberFromUser(messageAccept.u32BetLowerBounds, messageAccept.u32BetUpperBounds);
#else
    messageBet.u32BettingNumber = getRandomNumber(messageAccept.u32BetLowerBounds, messageAccept.u32BetUpperBounds, messageHeader.u16ClientID);
#endif

    fprintf(stderr, "[D] Betting Number: %x\n", messageBet.u32BettingNumber);
    nrBytesSent = send(socketDescriptor, &messageHeader, sizeof(messageHeader), 0);
    if (nrBytesSent == sizeof(messageHeader))
    {
        nrBytesSent = send(socketDescriptor, &messageBet, sizeof(messageBet), 0);
        if (nrBytesSent == sizeof(messageBet))
        {
            fprintf(stderr, "[I] Bet Message Sent Successfully!\n");
        }
    }

    recv(socketDescriptor, &messageHeader, sizeof(messageHeader), 0);
    recv(socketDescriptor, &messageResult, sizeof(messageResult), 0);

    if(messageResult.u8Status)
    {
        fprintf(stderr, "I WON!!!!\n");
    }
    else
    {
        fprintf(stderr, "I LOSE! :( \n");
    }

    
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
