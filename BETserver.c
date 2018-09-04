/* Filename: bettserver.c
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description: Server Implementation for BetServer
 //TODO: Add a more descriptive documentation
 */

/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "SOCKETwrapper.h"
#include "BETserver.h"

/*
 * MACROS
 */


/*
 * PRIVATE FUNCTIONS
 */

/*
 * PUBLIC FUNCTIONS
 */

int main(int argc, char const *argv[])
{
	/* code */
	SW_CreateServer(BETSERVER_PORT, BETSERVER_NUM_CLIENTS);
	return 0;
}