/* Filename: BETserver.c
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description: Server Implementation for BetServer
 */

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>

#include "SOCKETwrapper.h"
#include "BETserver_db.h"
#include "BETserver.h"

/*
 * MACROS
 */
//TODO: Check why stdout isnt flushed directly
//TODO: Research the correct stack size to set for a client thread
#define THREAD_STACKSIZE      (64 * 1024)
#ifdef RAND_MAX
  /* Suppress Redefinition Warning */
  #undef RAND_MAX
#endif
#define RAND_MAX              (BETSERVER_NUM_CLIENTS - 1U)

/*
 * GLOBAL VARIABLES
 */
int32_t s32ServerSocket;
bool isServerRunning = true;

/*
 * PRIVATE FUNCTION DECLARATION
 */
void handleInterruptSignal(int32_t signalNumber);
void createClientThread(uint32_t clientSocket, uint16_t clientID);
void *handleBetClient(void *data);
bool runServer(uint16_t serverPort);

/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */
void handleInterruptSignal(int32_t signalNumber)
{
    isServerRunning = false;
    SW_ShutdownServer(s32ServerSocket);
}

int32_t generateClientID(int32_t clientSocket, int32_t clientSockLength)
{
    //TODO: Move to DB Component
    uint16_t u16ClientID;
    tenuDBErrorCode tDBStatus;

    u16ClientID = rand();
    tDBStatus = DB_AddClientID(u16ClientID);

    switch (tDBStatus) {
        case DB_OK:
            fprintf(stderr, "[I] Client registered to Server!\n");
            break;
        case DB_DUPLICATE:
            fprintf(stderr, "[E] Client ID Duplicated, try again!\n");
            u16ClientID = DB_DUPLICATE;
            break;
        case DB_FULL:
            fprintf(stderr, "[E] Server DB Full!\n");
            u16ClientID = DB_FULL;
        default:
            break;
    }

    return u16ClientID;
}

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

    u64PassedArgument = (uint64_t) clientID;
    u64PassedArgument <<= 32;
    u64PassedArgument &= 0x0000FFFF00000000;
    u64PassedArgument |= (0x0000FFFF & clientSocket);

    threadCreationStatus = pthread_create(&clientThread, &threadAttributes, handleBetClient, (void *) u64PassedArgument);
    
    if (0 != threadCreationStatus)
    {
        fprintf(stderr, "[E] Could not Create Client Thread, Error Code: %d\n", threadCreationStatus);
    }
}
void *handleBetClient(void *data)
{
    char openMessage[6000];
    mBetServerMessageHeader messageHeader;

    uint32_t nrBytesRcvd= 0;
    uint32_t clientSocket = (uint32_t) (0x00000000FFFFFFFF & (uint64_t) data);
    uint32_t clientID = (uint32_t)    ((0xFFFFFFFF00000000 & (uint64_t) data) >> 32);

    fprintf(stdout, "[I] Thread Started for Client on Socket %d with ID %d\n", clientSocket, clientID);
    fflush(stdout);

    /* BETSERVER_OPEN */
    nrBytesRcvd = recv(clientSocket, &messageHeader, sizeof(openMessage), 0);

    if(messageHeader.u8Version != PROTOCOL_VERSION)
    {
        fprintf(stderr, "[E] Wrong Protocol Version, Disconnecting...\n");
        return NULL;
    }

    if(messageHeader.u8Type != BETSERVER_OPEN)
    {
        fprintf(stderr, "[E] Wrong Message Type\n");
        return NULL;
    }

    fprintf(stderr, "[I] Received Open Message from Client with ID %d\n", clientID);



    return NULL;
}
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
        clientSocket = accept(s32ServerSocket, (struct sockaddr *) &clientSockAddr, (socklen_t *) &clientSockLength);
        
        if (0 <= clientSocket)
        {
            clientID = generateClientID(clientSocket, clientSockLength);
            //TODO: Move inside DB
            while (clientID == DB_DUPLICATE)
            {
                fprintf(stderr,"[E] Duplicate Client ID, retrying\n");
                clientID = clientID = generateClientID(clientSocket, clientSockLength);
            }

            if(clientID != DB_FULL)
            {
                createClientThread(clientSocket, clientID);
            }
        }
    }
    
    return true;
}

/*
 * PUBLIC FUNCTIONS
 */
int main(int argc, char const *argv[])
{
    runServer(BETSERVER_PORT);
    return 0;
}
