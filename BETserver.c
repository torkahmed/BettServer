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
#include "BETserver.h"

/*
 * MACROS
 */
//TODO: Check why stdout isnt flushed directly
//TODO: Research the correct stack size to set for a client thread
#define THREAD_STACKSIZE      (64 * 1024)
/*
 * GLOBAL VARIABLES
 */
int32_t s32ServerSocket;
bool isServerRunning = true;

/*
 * PRIVATE FUNCTION DECLARATION
 */
void handleInterruptSignal(int32_t signalNumber);
void createClientThread(int32_t clientSocket, int32_t clientID);
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
    //TODO: Implement
    return 0;
}

void createClientThread(int32_t clientSocket, int32_t clientID)
{
    //TODO: Implement
    int32_t threadCreationStatus;
    pthread_t clientThread;
    pthread_attr_t threadAttributes;
    pthread_attr_init(&threadAttributes);
    pthread_attr_setstacksize(&threadAttributes, THREAD_STACKSIZE);

    threadCreationStatus = pthread_create(&clientThread, &threadAttributes, handleBetClient, (void *) clientID);
    
    if (0 != threadCreationStatus)
    {
        fprintf(stderr, "[E] Could not Create Client Thread, Error Code: %d\n", threadCreationStatus);
    }
}
void *handleBetClient(void *data)
{
    int32_t clientID = (int32_t) data;
    //TODO: Implement
    fprintf(stdout, "[I] Thread Started for Client with ID %d\n", clientID);
    fflush(stdout);
    
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
            createClientThread(clientSocket, clientID);
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
