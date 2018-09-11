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
#include <stdlib.h>

#include "BETserver_db.h"

/*
 * MACROS
 */


/*
 * GLOBAL VARIABLES
 */
LOCAL uint16_t clientIDList[BETSERVER_NUM_CLIENTS];
LOCAL uint32_t clientBettingNumbers[BETSERVER_NUM_CLIENTS];
LOCAL uint32_t numConnectedClients;
LOCAL bool winnerDetermined = false;
LOCAL uint32_t u32WinningNumber = 0;

/*
 * PRIVATE FUNCTION DECLARATION
 */


/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */

/*
 * PUBLIC FUNCTIONS
 */
PUBLIC tenuDBErrorCode DB_AppendClientID(uint16_t clientID)
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

PUBLIC bool DB_AddBettingNumber(uint16_t clientID, uint32_t bettingNumber)
{
    uint32_t i;
    for (i = 0; i<numConnectedClients; ++i)
    {
        if(clientIDList[i] == clientID)
        {
            clientBettingNumbers[i] = bettingNumber;
            return true;
        }
    }
    fprintf(stderr, "[E] Could not find Client ID\n");
    return false;
}

PUBLIC void DB_SelectWinningNumber(void)
{
    if(!winnerDetermined)
    {
        /* Generate a random number between 0 and numConnected Clients */
        int u8Index = rand() % numConnectedClients;
        fprintf(stderr, "[D] Random Winning Index: %d\n", u8Index);
        u32WinningNumber = clientBettingNumbers[u8Index];
        winnerDetermined = true;
    }
    return;
}

PUBLIC uint32_t DB_GetWinner(void)
{
    return u32WinningNumber;
}

PUBLIC void DB_ClearIDList(void)
{
    //TODO: Check how to call this from all threads, RESTART INSTANCE
    uint32_t i;

    for(i = 0; i < numConnectedClients; ++i)
    {
        clientIDList[i] = 0;
        clientBettingNumbers[i] = 0;
        winnerDetermined = false;
        u32WinningNumber = 0;
    }

    numConnectedClients = 0;
}

