/* Filename: bettserver.h
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description: Server Header for BetServer
 //TODO: Add a more descriptive documentation
 */

/*
 * INCLUDES
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/*
 * MACROS
 */
#define LOCAL static
#define PUBLIC
#define UNUSED(x) (void)(x)

#define BETSERVER_PORT          (2222)

#define BETSERVER_NUM_CLIENTS   (40U)
#define BETSERVER_NUM_MIN       (0xE0FFFF00)
#define BETSERVER_NUM_MAX       (0xE0FFFFAA)

/*
 * EXPORTED FUNCTIONS
 */