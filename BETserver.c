/* Filename: BETserver.c
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description: Server Implementation for BetServer
 */


/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include "SOCKETwrapper.h"
#include "BETserver_db.h"
#include "BETserver.h"


/*
 * MACROS
 */

/* 64KB of Stack Size */
#define THREAD_STACKSIZE      (64 * 1024)


/*
 * GLOBAL VARIABLES
 */

/* Server Socket */
LOCAL int32_t s32ServerSocket;
/* Flag to indicate if Server is currently running */
LOCAL bool isServerRunning = true;
/* Flag to indicate whether a betting run is already started, read/set by main thread, reset by timer thread */
LOCAL bool bTimerStarted = false;
/* Flag to indicate whether the betting run should end. Written by Timer Thread, Read by Client Threads */
LOCAL bool bTimeElapsed = false;


/*
 * PRIVATE FUNCTION DECLARATION
 */
void handleInterruptSignal(int32_t signalNumber);
bool checkMessageIntegrity(mBetServerMessageHeader tstrMessageHeader);
void createClientThread(uint32_t clientSocket, uint16_t clientID);
void createTimerThread(void);
void *handleBetClient(void *data);
void *startBettingRound(void *data);
void resetBettingRound(void);
bool runServer(uint16_t serverPort);


/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */

/*
 Function: handleInterruptSignal
 Description: Shutdown the server in case a signal is triggered (e.g User Interrupt)
 */
void handleInterruptSignal(int32_t signalNumber)
{
    isServerRunning = false;
    SW_TearConnection(s32ServerSocket);
}

/*
 Function: checkMessageIntegrity
 Description: Check that the message does not violate the protocol specs
 */
bool checkMessageIntegrity(mBetServerMessageHeader tstrMessageHeader)
{
    bool result = true;
    if(tstrMessageHeader.u8Version != PROTOCOL_VERSION)
    {
        fprintf(stderr, "[E] Wrong Protocol Version\n");
        result = false;
    }
    switch(tstrMessageHeader.u8Type)
    {
    case (BETSERVER_OPEN):
            if(tstrMessageHeader.u8Length != sizeof(mBetServerMessageHeader))
            {
                fprintf(stderr, "[E] Wrong Open Message Length\n");
                result = false;
            }
            break;
    case (BETSERVER_ACCEPT):
            if(tstrMessageHeader.u8Length != (sizeof(mBetServerMessageHeader) + sizeof(mBetServerMessageAccept)))
            {
                fprintf(stderr, "[E] Wrong Accept Message Length\n");
                result = false;
            }
            break;
    case (BETSERVER_BET):
            if(tstrMessageHeader.u8Length != (sizeof(mBetServerMessageHeader) + sizeof(mBetServerMessageBet)))
            {
                fprintf(stderr, "[E] Wrong Bet Message Length\n");
                result = false;
            }
            break;
    case (BETSERVER_RESULT):
            if(tstrMessageHeader.u8Length != (sizeof(mBetServerMessageHeader) + sizeof(mBetServerMessageResult)))
            {
                fprintf(stderr, "[E] Wrong Result Message Length\n");
                result = false;
            }
            break;
    default:
            result = false;
            break;
    }
    return result;
}

/*
 Function: createClientThread
 Description: Function to create a new pthread instance for the newly connected client
 */
void createClientThread(uint32_t clientSocket, uint16_t clientID)
{
    /*    u64PassedArgument
     *  AAAAAAAA    BBBBBBBB
     *  clientID  clientSocket
     */
    uint64_t u64PassedArgument = 0;
    int32_t threadCreationStatus;
    pthread_t clientThread;
    pthread_attr_t threadAttributes;
    pthread_attr_init(&threadAttributes);
    pthread_attr_setstacksize(&threadAttributes, THREAD_STACKSIZE);

    /*
     This was implemented as a workaround to pthread limitation of one argument, further research could be done to add extra arguments to function.
     */
    /* Concatenate both u32 variables into a u64 to pass as one parameter to clientThread */
    u64PassedArgument = (uint64_t) clientID;
    u64PassedArgument <<= 32;
    u64PassedArgument &= 0x0000FFFF00000000;
    u64PassedArgument |= (0x0000FFFF & clientSocket);

    threadCreationStatus = pthread_create(&clientThread, &threadAttributes, handleBetClient, (void *) u64PassedArgument);
    
    if (0 != threadCreationStatus)
    {
        fprintf(stderr, "[E] Could not Create Client Thread, Error Code: %d\n", threadCreationStatus);
    }
    
    return;
}

/*
 Function: createTimerThread
 Description: Function to start a separate thread to start/stop counting the 15 sec interval (Betting Instance)
 */
void createTimerThread(void)
{
    int32_t threadCreationStatus;
    pthread_t timerThread;
    pthread_attr_t threadAttributes;
    pthread_attr_init(&threadAttributes);
    pthread_attr_setstacksize(&threadAttributes, THREAD_STACKSIZE);

    threadCreationStatus = pthread_create(&timerThread, &threadAttributes, startBettingRound, (void *) NULL);

    if (0 != threadCreationStatus)
    {
        fprintf(stderr, "[E] Could not Create Timer Thread, Error Code: %d\n", threadCreationStatus);
    }
}

/*
 Function: handleBetClient
 Description: thread Function. When the thread is started, this function will be called.
 Handles the communication between the server and one client per thread
 */
void *handleBetClient(void *data)
{
    mBetServerMessageHeader messageHeader;
    mBetServerMessageAccept messageAccept;
    mBetServerMessageBet messageBet;
    mBetServerMessageResult messageResult;

    uint32_t nrBytesRcvd= 0;
    uint32_t nrBytesSent = 0;
    uint32_t clientSocket = (uint32_t) (0x00000000FFFFFFFF & (uint64_t) data);
    uint32_t clientID = (uint32_t)    ((0xFFFFFFFF00000000 & (uint64_t) data) >> 32);

    fprintf(stdout, "[I] Thread Started for Client on Socket %d with ID %d\n", clientSocket, ntohs(clientID));
    fflush(stdout);

    /*                          BETSERVER_OPEN                                */
    /* Receive Header */
    /* TODO: BUG Extra Byte being sent in Header Message */
    nrBytesRcvd = recv(clientSocket, &messageHeader, sizeof(messageHeader), 0);
    if(nrBytesRcvd != sizeof(messageHeader))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Received\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Validate Message */
    if((messageHeader.u8Type != BETSERVER_OPEN) || !checkMessageIntegrity(messageHeader) )
    {
        fprintf(stderr, "[E] Protocol Violation by Client... Disconnecting\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /*                         BETSERVER_ACCEPT                               */

    /* Prepare Message */
    messageHeader.u8Type = BETSERVER_ACCEPT;
    messageHeader.u8Length = sizeof(mBetServerMessageHeader) + sizeof(mBetServerMessageAccept);
    messageHeader.u16ClientID = clientID;
    messageAccept.u32BetLowerBounds = BETSERVER_NUM_MIN;
    messageAccept.u32BetUpperBounds = BETSERVER_NUM_MAX;

    /* Send Header */
    nrBytesSent = send(clientSocket, &messageHeader, sizeof(messageHeader), 0);
    if(!(nrBytesSent == sizeof(messageHeader)))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Sent\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Send Payload */
    nrBytesSent = send(clientSocket, &messageAccept, sizeof(messageAccept), 0);
    if(nrBytesSent != sizeof(messageAccept))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Sent\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /*                         BETSERVER_BET                                  */

    /* Receive Header */
    nrBytesRcvd = recv(clientSocket, &messageHeader, sizeof(messageHeader), 0);
    if(nrBytesRcvd != sizeof(messageHeader))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Received\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Receive Payload */
    nrBytesRcvd = recv(clientSocket, &messageBet, sizeof(messageBet), 0);
    if(nrBytesRcvd != sizeof(messageBet))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Received\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Validate Message */
    if((messageHeader.u8Type != BETSERVER_BET) || !checkMessageIntegrity(messageHeader) )
    {
        fprintf(stderr, "[E] Protocol Violation by Client... Disconnecting\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Update Database with Betting Number for this Client ID */
    if(!DB_AddBettingNumber(messageHeader.u16ClientID, messageBet.u32BettingNumber))
    {
        fprintf(stderr, "[E] Could not find Client ID in Database!\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Wait in thread till timer is elapsed */
    while(!bTimeElapsed)
    {
        /* Do Nothing */
    }

    /*                       BETSERVER_RESULT                                 */

    /* Prepare Message */
    messageHeader.u8Length = sizeof(messageHeader) + sizeof(messageResult);
    messageHeader.u8Type = BETSERVER_RESULT;
    messageResult.u32WinningNumber = DB_GetWinner();

    if(messageResult.u32WinningNumber == messageBet.u32BettingNumber)
    {
        messageResult.u8Status = true;
        fprintf(stderr, "[I] Client ID: %d Won!\n", ntohs(clientID));
    }
    else
    {
        messageResult.u8Status = false;
        fprintf(stderr, "[I] Client ID: %d Lost!\n", ntohs(clientID));
    }

    /* Send Header */
    nrBytesSent = send(clientSocket, &messageHeader, sizeof(messageHeader), 0);
    if(!(nrBytesSent == sizeof(messageHeader)))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Sent\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Send Payload */
    nrBytesSent = send(clientSocket, &messageResult, sizeof(messageResult), 0);
    if(!(nrBytesSent == sizeof(messageResult)))
    {
        fprintf(stderr, "[E] Incorrect Number of Bytes Sent\n");
        SW_TearConnection(clientSocket);
        return NULL;
    }

    /* Tell DB Component that this client is served */
    if(!DB_ClientIsFinished(clientID))
    {
        fprintf(stderr, "[E] Could not find Client in Database\n");
    }

    /* After Serving the client, tear down the connection */
    SW_TearConnection(clientSocket);
    return NULL;
}

/*
 Function: startBettingRound
 Description: Starts counting the 15 sec interval for the betting round. when all clients are served, resets the data to make it possible to start a new betting round on a new client connection
 */
void *startBettingRound(void *data)
{
    /* Wait for 15 seconds */
    fprintf(stdout, "[I] Betting Instance Started \n");
    sleep(15U);

    /* Select the Winner */
    DB_SelectWinningNumber();
    fprintf(stderr, "[I] The Winning Number is %x!\n", ntohl(DB_GetWinner()));

    /* Allow Client threads to send results to clients */
    bTimeElapsed = true;

    while(!DB_AllClientsServed())
    {
        /* Wait here until All Clients are served */
    }

    /* Cleanup the old winning round and prepare for the next one */
    resetBettingRound();
    fprintf(stderr, "[I] Betting Instance Ended!\n");
    return NULL;
}

/*
 Function: resetBettingRound
 Description: Reset the variables to make it possible to start a new betting round on next client connection
 */
void resetBettingRound(void)
{
    /* Clear Database of existing Clients */
    DB_ClearIDList();

    /* Reset Instancing flags */
    bTimeElapsed = false;
    bTimerStarted = false;
}

/*
 Function: runServer
 Description: Betting Server Initialization and running. This function runs in the main thread and never exits
 */
bool runServer(uint16_t serverPort)
{
    struct sockaddr_in clientSockAddr;
    int32_t clientSocket;
    int32_t clientSockLength;
    int32_t clientID;
    
    /* Monitor Interrupt/Termination Signals to Close Socket */
    signal(SIGINT, handleInterruptSignal);
    signal(SIGTERM, handleInterruptSignal);
    signal(SIGSEGV, handleInterruptSignal);
    
    /* Create Bet Server on selected Port */
    s32ServerSocket = SW_CreateServer(serverPort, BETSERVER_NUM_CLIENTS);
    
    if (s32ServerSocket == INVALID_SOCKET)
    {
        return false;
    }
    
    while(isServerRunning)
    {
        /*
         The addrlen argument is a value-result argument: the caller must
         initialize it to contain the size (in bytes) of the structure pointed
         to by addr; on return it will contain the actual size of the peer
         address.
         */
        clientSockLength = sizeof(clientSocket);

        /* Blocking call, wait for clients to connect */
        clientSocket = accept(s32ServerSocket, (struct sockaddr *) &clientSockAddr, (socklen_t *) &clientSockLength);
        
        if (0 <= clientSocket)
        {
            /* Check if a betting instance is already running, if not then start a new one */
            if(!bTimerStarted)
            {
                createTimerThread();
                bTimerStarted = true;
            }

            /* Add the newly connected client to the database */
            clientID = DB_AppendClient(clientSocket, clientSockLength);

            /* if the number of connected clients is still less than BETSERVER_NUM_CLIENTS,
             *  then start communication with the new client */
            if(clientID != DB_FULL)
            {
                createClientThread(clientSocket, clientID);
            }
        }
    }
    
    return true;
}


/*
 * MAIN FUNCTION
 */
int main(int argc, char const *argv[])
{
    runServer(BETSERVER_PORT);
    return 0;
}
