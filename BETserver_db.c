/* Filename: BETserver_db.c
 * Author: Ahmed Tourk
 * Date: 05.09.2018
 * Description: Client Implementation for BetServer
 */

/*
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "BETserver_db.h"

/*
 * MACROS
 */


/*
 * GLOBAL VARIABLES
 */
uint16_t clientIDList[BETSERVER_NUM_CLIENTS];
uint32_t numConnectedClients;

/*
 * PRIVATE FUNCTION DECLARATION
 */


/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */

/*
 * PUBLIC FUNCTIONS
 */
PUBLIC tenuDBErrorCode DB_AddClientID(uint16_t clientID)
{
    int i;
    /* 1. Check that ID List isn't full */
    if(numConnectedClients >= BETSERVER_NUM_CLIENTS)
    {
        return DB_FULL;
    }

    /* 2. Check that ID is not duplicated */
    for(i = 0; i < numConnectedClients; ++i)
    {
        if(clientIDList[i] == clientID)
        {
            return DB_DUPLICATE;
        }
    }

    /* 3. Add Client to List */
    clientIDList[numConnectedClients] = clientID;
    numConnectedClients++;
    fprintf(stderr, "[I] Number of Connected Clients: %d, Remaining: %d\n", numConnectedClients, (BETSERVER_NUM_CLIENTS - numConnectedClients));

    return DB_OK;

}

PUBLIC void DB_ClearIDList(void)
{
    uint32_t i;

    for(i = 0; i < numConnectedClients; ++i)
    {
        clientIDList[i] = 0;
    }

    numConnectedClients = 0;
}

