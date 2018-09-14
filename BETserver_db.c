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
LOCAL bool isClientFinished[BETSERVER_NUM_CLIENTS];
LOCAL uint32_t numConnectedClients;
LOCAL bool winnerDetermined = false;
LOCAL uint32_t u32WinningNumber = 0;

/*
 * PRIVATE FUNCTION DECLARATION
 */
LOCAL tenuDBErrorCode appendIdToList(uint16_t clientID);

/*
 * PRIVATE FUNCTION IMPLEMENTATION
 */

LOCAL tenuDBErrorCode appendIdToList(uint16_t clientID)
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

/*
 * PUBLIC FUNCTIONS
 */

PUBLIC int32_t DB_AppendClient(int32_t clientSocket, int32_t clientSockLength)
{
    uint16_t u16ClientID;
    tenuDBErrorCode tDBStatus;
    
    do
    {
        u16ClientID = rand() % (BETSERVER_NUM_CLIENTS - 1);
        tDBStatus = appendIdToList(u16ClientID);
        
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
    } while (tDBStatus == DB_DUPLICATE);
    
    return u16ClientID;
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
    /*
     * This Method takes a random betting number from the array of client betting numbers
     * This will not be a 100% fair chance if two clients use the same betting number
     * e.g {0xAA, 0xBB, 0xAA, 0xCC}
     * The chances to pick 0xAA is double the chances to pick 0xBB or 0xCC
     * Possible Improvement: Save the betting number in a different array that would take one entry per number
     * and randomize a winner from that array
     */
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

PUBLIC bool DB_ClientIsFinished(uint16_t clientID)
{
    uint32_t i;
    for (i = 0; i<numConnectedClients; ++i)
    {
        if(clientIDList[i] == clientID)
        {
            isClientFinished[i] = true;
            return true;
        }
    }
    fprintf(stderr, "[E] Could not find Client ID\n");
    return false;

}

PUBLIC bool DB_AllClientsServed(void)
{
    uint8_t i;
    for(i = 0; i < numConnectedClients; ++i)
    {
        if(!isClientFinished[i])
        {
            return false;
        }
    }
    return true;
}

PUBLIC void DB_ClearIDList(void)
{
    uint32_t i;

    for(i = 0; i < numConnectedClients; ++i)
    {
        clientIDList[i] = 0;
        clientBettingNumbers[i] = 0;
        isClientFinished[i] = false;
        winnerDetermined = false;
        u32WinningNumber = 0;
    }

    numConnectedClients = 0;
}

