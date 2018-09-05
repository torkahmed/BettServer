/* Filename: BETserver.h
 * Author: Ahmed Tourk
 * Date: 04.09.2018
 * Description:
 */

/*
 * INCLUDES
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "BETstd.h"


/*
 * MACROS
 */
#define BETSERVER_PORT          (2222)

#define BETSERVER_NUM_CLIENTS   (40U)
#define BETSERVER_NUM_MIN       (0xE0FFFF00)
#define BETSERVER_NUM_MAX       (0xE0FFFFAA)

/*
 * APIs
 */
