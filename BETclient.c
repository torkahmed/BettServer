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
bool clientConnectToServer(char *serverIP, int16_t serverPort);

/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */
bool clientConnectToServer(char *serverHumanRIP, int16_t serverPort)
{
    uint32_t serverIP = 0;
    int32_t socketDescriptor;

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



}

/*
 * PUBLIC FUNCTIONS
 */

int main(int argc, char const *argv[])
{

}
